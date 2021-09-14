#include <sys/stat.h>
#include "gss_bind.h"
#ifdef __MVS__
#include "krb5_zos.h"
#endif

// https://docs.microsoft.com/en-us/troubleshoot/windows-server/group-policy/group-policy-add-maxtokensize-registry-entry
// Max token size in most recent Microsoft OS is 48 000 before B64
// the hard limit is set to 65 535 but discouraged because of B64 conversion that is around +30% in size
const size_t MAX_AD_TOKEN_SIZE_BEFORE_B64 = 48000;

static gss_OID_desc _gss_mech_spnego = {6, (void *)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO = &_gss_mech_spnego;                       //gss_OID == gss_OID_desc*

void _convert_gss_error(const OM_uint32 gss_err, const OM_uint32 gss_minor, std::string &error_msg);

bool exists(const char *path)
{
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

/**
 * generate_spnego_token
 */
class Worker_generate_spnego_token : public Napi::AsyncWorker
{
public:
  Worker_generate_spnego_token(std::string str_server,
                               std::string gss_name_type,
                               std::string ccname,
                               Napi::Function &callback)
      : Napi::AsyncWorker(callback), str_server(str_server),
        input_name_type(gss_name_type),
        krb_ccname(ccname),
        error_msg("")
  {
  }

private:
  OM_uint32 import_name(const char *principal, gss_name_t *desired_name)
  {
    gss_buffer_desc service;
    service.length = strlen(principal);
    service.value = (char *)principal;
    auto name_type = (input_name_type == "GSS_C_NT_USER_NAME") ? GSS_C_NT_USER_NAME
                                                               : GSS_C_NT_HOSTBASED_SERVICE;
    return gss_import_name(&gss_minor, &service, name_type, desired_name);
  }

  void Execute()
  {
#ifdef __MVS__
    __ae_runmode rm(__AE_ASCII_MODE);
#endif
    const char *server = str_server.c_str();
    gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
    gss_buffer_desc input_buf, output_buf;
    gss_name_t target_name;
    gss_err = import_name(server, &target_name);
    if (gss_err)
    {
      error_msg = "Error while importing name.";
      return;
    }

    if (krb_ccname != "")
    {
      // gss_krb5_ccache_name is actually thread safe,
      // as discussed in https://krbdev.mit.narkive.com/hOSNjHRA/krb5-get-in-tkt-with-password-gss-init-sec-context
      gss_err = gss_krb5_ccache_name(&gss_minor, krb_ccname.c_str(), NULL);
    }

    gss_err = gss_init_sec_context(&gss_minor,
                                   GSS_C_NO_CREDENTIAL, // uses ccache specified with gss_krb5_ccache_name or default
                                   &gss_context,
                                   target_name,
                                   GSS_MECH_SPNEGO,
                                   GSS_C_REPLAY_FLAG | GSS_C_SEQUENCE_FLAG,
                                   GSS_C_INDEFINITE,
                                   GSS_C_NO_CHANNEL_BINDINGS,
                                   &input_buf,
                                   NULL,
                                   &output_buf,
                                   NULL,
                                   NULL);

    if (!(GSS_ERROR(gss_err)))
    {
      if (output_buf.length > MAX_AD_TOKEN_SIZE_BEFORE_B64)
      {
        this->error_msg += "The token returned by GSS is greater than the size allowed by Windows AD";
        this->spnego_token_length = 0;
        this->spnego_token = NULL;
      }
      else
      {
        this->spnego_token_length = output_buf.length;
        this->spnego_token = new uint8_t[this->spnego_token_length];
        memcpy((void *)this->spnego_token, output_buf.value, output_buf.length);
      }
    }
    else
    {
      _convert_gss_error(gss_err, gss_minor, this->error_msg);
      this->spnego_token_length = 0;
      this->spnego_token = NULL;
    }
    gss_err = gss_delete_sec_context(&gss_minor, &gss_context, &output_buf);
    if (GSS_ERROR(gss_err))
    {
      _convert_gss_error(gss_err, gss_minor, this->error_msg);
    }
  }

  void OnOK()
  {
    Napi::HandleScope scope(Env());

    Callback().Call({Napi::String::New(Env(), error_msg),
                     (spnego_token) ? Napi::Buffer<uint8_t>::New(Env(), this->spnego_token, this->spnego_token_length, [](Napi::Env env, uint8_t *ref)
                                                                 { free(ref); })
                                    : Env().Undefined()});
  }

  //
  OM_uint32 gss_err;
  OM_uint32 gss_minor;

  // In parameter
  std::string str_server;
  std::string input_name_type;
  std::string krb_ccname;

  // Out parameter
  uint8_t *spnego_token;
  size_t spnego_token_length;
  std::string error_msg;
};

void _convert_gss_error(const OM_uint32 gss_err, const OM_uint32 gss_minor, std::string &error_msg)
{
  if (error_msg.length() > 0)
  {
    error_msg += "\n";
  }
  OM_uint32 message_context;
  OM_uint32 min_status;
  gss_buffer_desc status_string;
  message_context = 0;
  do
  {
    gss_display_status(&min_status,
                       gss_err,
                       GSS_C_GSS_CODE,
                       GSS_C_NO_OID,
                       &message_context,
                       &status_string);
    error_msg += (char *)status_string.value;
    gss_release_buffer(&min_status, &status_string);
  } while (message_context != 0);
  error_msg += " (minor ";
  error_msg += std::to_string(gss_minor);
  error_msg += ")";
}

Napi::Value _generate_spnego_token(const Napi::CallbackInfo &info)
{
  if (info.Length() < 4)
  {
    throw Napi::TypeError::New(info.Env(), "4 arguments expected");
  }

  std::string server = info[0].As<Napi::String>().Utf8Value();
  std::string input_name_type = info[1].As<Napi::String>().Utf8Value();
  std::string ccname = info[2].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[3].As<Napi::Function>();

  Worker_generate_spnego_token *worker = new Worker_generate_spnego_token(server,
                                                                          input_name_type,
                                                                          ccname,
                                                                          callback);
  worker->Queue();
  return info.Env().Undefined();
}

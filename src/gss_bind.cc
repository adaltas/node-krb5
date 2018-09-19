#include <sys/stat.h>
#include "gss_bind.h"

static gss_OID_desc _gss_mech_spnego = {6, (void*)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO= &_gss_mech_spnego; //gss_OID == gss_OID_desc*

bool exists(const char* path){
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}


/**
 * generate_spnego_token
 */
class Worker_generate_spnego_token : public Napi::AsyncWorker {
  public:
    Worker_generate_spnego_token(std::string str_server,
                                 Napi::Function& callback)
      : Napi::AsyncWorker(callback), str_server(str_server) {
    }

  private:
    OM_uint32 import_name(const char* principal, gss_name_t* desired_name) {
      OM_uint32 ret;
      gss_buffer_desc service;
      service.length = strlen(principal);
      service.value = (char*)principal;
      ret=gss_import_name((OM_uint32*)&this->err, &service,GSS_C_NT_HOSTBASED_SERVICE, desired_name);
      return ret;
    }

    void Execute() {
      const char* server = str_server.c_str();
      gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
      gss_buffer_desc input_buf,output_buf;
      gss_name_t target_name;
      gss_err = import_name(server,&target_name);
      if(gss_err) {
        return;
      }
      gss_err = gss_init_sec_context((OM_uint32*)&this->err,
                      GSS_C_NO_CREDENTIAL,
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
      if(!(GSS_ERROR(gss_err) || this->err)){
        char token_buffer[2048];
        encode64((char*)output_buf.value,token_buffer,output_buf.length);
        this->spnego_token = new char[strlen(token_buffer)+1];
        strcpy(this->spnego_token, token_buffer);
      }
      else{
        if(GSS_ERROR(gss_err) && !this->err){
          char token_buffer[2048];
          OM_uint32 message_context;
          OM_uint32 min_status;
          gss_buffer_desc status_string;
          message_context = 0;
          token_buffer[0] = '\0';
          do {
            gss_display_status(
                  &min_status,
                  gss_err,
                  GSS_C_GSS_CODE,
                  GSS_C_NO_OID,
                  &message_context,
                  &status_string);
            strcat(token_buffer, (char *)status_string.value);
            gss_release_buffer(&min_status, &status_string);
          } while (message_context != 0);
          //this->init_custom_error(gss_err,token_buffer);
          //this->set_error(gss_err);
        }
        this->spnego_token = NULL;
      }
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), GSS_ERROR(gss_err)),
        Napi::Number::New(Env(), err),
        (spnego_token) ? Napi::String::New(Env(), spnego_token) : Env().Undefined()
      });
    }

    //
    krb5_error_code err;

    // In parameter
    std::string str_server;

    // Out parameter
    OM_uint32 gss_err;
    char* spnego_token;
};

Napi::Value _generate_spnego_token(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
    throw Napi::TypeError::New(info.Env(), "2 arguments expected");
  }

  std::string server = info[0].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_generate_spnego_token* worker = new Worker_generate_spnego_token(server, callback);
  worker->Queue();
  return info.Env().Undefined();
}


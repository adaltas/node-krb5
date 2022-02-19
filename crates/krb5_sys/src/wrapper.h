#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>

const gss_cred_id_t _GSS_C_NO_CREDENTIAL = GSS_C_NO_CREDENTIAL;
#undef GSS_C_NO_CREDENTIAL
const gss_cred_id_t GSS_C_NO_CREDENTIAL = _GSS_C_NO_CREDENTIAL;

const gss_channel_bindings_t _GSS_C_NO_CHANNEL_BINDINGS = GSS_C_NO_CHANNEL_BINDINGS;
#undef GSS_C_NO_CHANNEL_BINDINGS
const gss_channel_bindings_t GSS_C_NO_CHANNEL_BINDINGS = _GSS_C_NO_CHANNEL_BINDINGS;

const gss_ctx_id_t _GSS_C_NO_CONTEXT = GSS_C_NO_CONTEXT;
#undef GSS_C_NO_CONTEXT
const gss_ctx_id_t GSS_C_NO_CONTEXT = _GSS_C_NO_CONTEXT;

#define GSS_ERROR(x) \
  ((x) & ((GSS_C_CALLING_ERROR_MASK << GSS_C_CALLING_ERROR_OFFSET) | \
          (GSS_C_ROUTINE_ERROR_MASK << GSS_C_ROUTINE_ERROR_OFFSET)))

OM_uint32 gss_error(OM_uint32 x) {
    return GSS_ERROR(x);
}

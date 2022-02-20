#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>

#undef GSS_C_NO_CREDENTIAL
const gss_cred_id_t GSS_C_NO_CREDENTIAL = ((gss_cred_id_t) 0);

#undef GSS_C_NO_CHANNEL_BINDINGS
const gss_channel_bindings_t GSS_C_NO_CHANNEL_BINDINGS = ((gss_channel_bindings_t) 0);

#undef GSS_C_NO_CONTEXT
const gss_ctx_id_t GSS_C_NO_CONTEXT = ((gss_ctx_id_t) 0);

OM_uint32 gss_error(OM_uint32 x) {
    return GSS_ERROR(x);
}

gss_OID gss_c_nt_hostbased_service() {
    return GSS_C_NT_HOSTBASED_SERVICE;
}

gss_OID gss_c_nt_user_name() {
    return GSS_C_NT_USER_NAME;
}

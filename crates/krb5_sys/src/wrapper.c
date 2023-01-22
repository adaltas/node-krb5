#include "wrapper.h"

OM_uint32 gss_error(OM_uint32 x) { return GSS_ERROR(x); }

gss_OID gss_c_nt_hostbased_service() { return GSS_C_NT_HOSTBASED_SERVICE; }

gss_OID gss_c_nt_user_name() { return GSS_C_NT_USER_NAME; }

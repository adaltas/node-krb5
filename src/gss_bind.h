#include <napi.h>
#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>
#include <string>

#include "base64.h"

Napi::Value _generate_spnego_token(const Napi::CallbackInfo& info);

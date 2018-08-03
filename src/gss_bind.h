#include <napi.h>
#include <krb5.h>
#include <string>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>
#include "base64.h"
//For debug
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>


Napi::Value _generate_spnego_token(const Napi::CallbackInfo& info);
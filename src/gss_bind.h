#include <napi.h>
#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>
#include <string>
#include "base64.h"
//For debug
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef __MVS__
#include <sys/syscall.h>
#endif

Napi::Value _generate_spnego_token(const Napi::CallbackInfo& info);

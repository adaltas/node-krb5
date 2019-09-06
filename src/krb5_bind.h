#include <napi.h>
#include <krb5.h>
#include <string>
//For debug
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef __MVS__
#include <sys/syscall.h>
#endif

Napi::Value _krb5_build_principal(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_close(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_default(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_destroy(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_get_name_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_initialize(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_initialize_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_resolve(const Napi::CallbackInfo& info);
Napi::Value _krb5_cc_store_cred(const Napi::CallbackInfo& info);
Napi::Value _krb5_free_context(const Napi::CallbackInfo& info);
Napi::Value _krb5_free_context_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_free_creds_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_free_principal_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_get_default_realm(const Napi::CallbackInfo& info);
Napi::Value _krb5_get_error_message_sync(const Napi::CallbackInfo& info);
Napi::Value _krb5_get_init_creds_keytab(const Napi::CallbackInfo& info);
Napi::Value _krb5_get_init_creds_password(const Napi::CallbackInfo& info);
Napi::Value _krb5_init_context(const Napi::CallbackInfo& info);
Napi::Value _krb5_kt_resolve(const Napi::CallbackInfo& info);

#include "krb5_bind.h"


Napi::Object init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "krb5_build_principal"), Napi::Function::New(env, _krb5_build_principal));
    exports.Set(Napi::String::New(env, "krb5_cc_default"), Napi::Function::New(env, _krb5_cc_default));
    exports.Set(Napi::String::New(env, "krb5_cc_get_name_sync"), Napi::Function::New(env, _krb5_cc_get_name_sync));
    exports.Set(Napi::String::New(env, "krb5_cc_initialize"), Napi::Function::New(env, _krb5_cc_initialize));
    exports.Set(Napi::String::New(env, "krb5_cc_initialize_sync"), Napi::Function::New(env, _krb5_cc_initialize_sync));
    exports.Set(Napi::String::New(env, "krb5_free_context"), Napi::Function::New(env, _krb5_free_context));
    exports.Set(Napi::String::New(env, "krb5_get_default_realm"), Napi::Function::New(env, _krb5_get_default_realm));
    exports.Set(Napi::String::New(env, "krb5_init_context"), Napi::Function::New(env, _krb5_init_context));

    return exports;
};

NODE_API_MODULE(NODE_GYP_MODULE_NAME, init);
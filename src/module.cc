#include "krb5_bind.h"
#include "gss_bind.h"


Napi::Object init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "krb5_build_principal"), Napi::Function::New(env, _krb5_build_principal));
    exports.Set(Napi::String::New(env, "krb5_cc_close"), Napi::Function::New(env, _krb5_cc_close));
    exports.Set(Napi::String::New(env, "krb5_cc_default"), Napi::Function::New(env, _krb5_cc_default));
    exports.Set(Napi::String::New(env, "krb5_cc_destroy"), Napi::Function::New(env, _krb5_cc_destroy));
    exports.Set(Napi::String::New(env, "krb5_cc_get_name_sync"), Napi::Function::New(env, _krb5_cc_get_name_sync));
    exports.Set(Napi::String::New(env, "krb5_cc_initialize"), Napi::Function::New(env, _krb5_cc_initialize));
    exports.Set(Napi::String::New(env, "krb5_cc_initialize_sync"), Napi::Function::New(env, _krb5_cc_initialize_sync));
    exports.Set(Napi::String::New(env, "krb5_cc_resolve"), Napi::Function::New(env, _krb5_cc_resolve));
    exports.Set(Napi::String::New(env, "krb5_cc_store_cred"), Napi::Function::New(env, _krb5_cc_store_cred));
    exports.Set(Napi::String::New(env, "krb5_free_context"), Napi::Function::New(env, _krb5_free_context));
    exports.Set(Napi::String::New(env, "krb5_free_context_sync"), Napi::Function::New(env, _krb5_free_context_sync));
    exports.Set(Napi::String::New(env, "krb5_free_creds_sync"), Napi::Function::New(env, _krb5_free_creds_sync));
    exports.Set(Napi::String::New(env, "krb5_free_principal_sync"), Napi::Function::New(env, _krb5_free_principal_sync));
    exports.Set(Napi::String::New(env, "krb5_get_default_realm"), Napi::Function::New(env, _krb5_get_default_realm));
    exports.Set(Napi::String::New(env, "krb5_get_error_message_sync"), Napi::Function::New(env, _krb5_get_error_message_sync));
    exports.Set(Napi::String::New(env, "krb5_get_init_creds_keytab"), Napi::Function::New(env, _krb5_get_init_creds_keytab));
    exports.Set(Napi::String::New(env, "krb5_get_init_creds_password"), Napi::Function::New(env, _krb5_get_init_creds_password));
    exports.Set(Napi::String::New(env, "krb5_init_context"), Napi::Function::New(env, _krb5_init_context));
    exports.Set(Napi::String::New(env, "krb5_kt_resolve"), Napi::Function::New(env, _krb5_kt_resolve));

    exports.Set(Napi::String::New(env, "generate_spnego_token"), Napi::Function::New(env, _generate_spnego_token));

    return exports;
};

NODE_API_MODULE(NODE_GYP_MODULE_NAME, init);

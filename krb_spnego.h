#ifndef KRB_SPNEGO_H
#define KRB_SPNEGO_H
//std libs
#include <iostream>
#include <string>
#include <string.h>

// V8/Node libs
#include <v8.h>
#include <uv.h>
#include <node/node.h>
#include <node/node_object_wrap.h>
//Kerberos libs
#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>

//local libs
#include "base64.h"

#define NO_ERROR 1000

using namespace v8;

class KrbSpnego : node::ObjectWrap {
private:
  std::string realm;
  std::string spnego_token;
  krb5_context context;
  krb5_keytab keytab;
  krb5_ccache cache;
  krb5_creds* cred;
  krb5_principal client_principal;
  gss_cred_id_t gss_cred;
  void krb5_cleanup(int level);
  void krb5_error(int level, char const* mesg);
  OM_uint32 import_name(OM_uint32* minor_status,std::string& server, gss_name_t* desired_name);
  krb5_error_code krb5_finish_get_cred();

public:
  std::string getRealm();
  KrbSpnego(std::string& user, std::string& realm);
  virtual ~KrbSpnego();
  krb5_error_code krb5_get_credentials_by_keytab(std::string& keytabName);
  krb5_error_code krb5_get_credentials_by_password(std::string& password);
  OM_uint32 generate_spnego_token(std::string& server);
  std::string getSpnegoToken();
  ///Binding
  static v8::Persistent<v8::FunctionTemplate> tpl;
  //library initialization
  static void Initialize(Handle<Object> target);
  static v8::Handle<v8::Value> getToken(Local<v8::String> property, const v8::AccessorInfo& info);
  //async bindings
  static v8::Handle<Value> ByPassword(const Arguments& args);
  static v8::Handle<Value> ByKeyTab(const Arguments& args);
  static v8::Handle<Value> GenToken(const Arguments& args);
  //sync bindings
  static v8::Handle<Value> ByPasswordSync(const Arguments& args);
  static v8::Handle<Value> ByKeyTabSync(const Arguments& args);
  static v8::Handle<Value> GenTokenSync(const Arguments& args);
  //Constructor Binding
  static v8::Handle<Value> New(const Arguments& args);
};

v8::Persistent<FunctionTemplate> KrbSpnego::tpl;

//boilerplate code

extern "C" { // Cause of name mangling in C++, we use extern C here
  static void init(Handle<Object> target) {
    KrbSpnego::Initialize(target);
  }
  // @see http://github.com/ry/node/blob/v0.2.0/src/node.h#L101
  NODE_MODULE(krb_spnego, init);
}

#endif // KRB_SPNEGO_H

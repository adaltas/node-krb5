#ifndef KRB_SPNEGO_H
#define KRB_SPNEGO_H
//std libs
#include <iostream>
#include <cstdio>
#include <string.h>
#ifdef NODEGYP
// V8/Node libs
#include <v8.h>
#include <uv.h>
#include <node.h>
#include <node_object_wrap.h>
#endif
//Kerberos libs
#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>


//local libs
#include "base64.h"

#ifdef NODEGYP
using namespace v8;

class Krb5 : node::ObjectWrap {
#else
class Krb5 {
#endif
private:
  bool renew;
  krb5_ccache cache;
  krb5_creds* cred;
  krb5_principal client_principal;
  krb5_error_code cleanup(int level);
  void krb5_error(int level, char const* mesg);
  OM_uint32 import_name(const char* server, gss_name_t* desired_name);

public:
  krb5_context context;
  krb5_error_code err;
  char* spnego_token;
  Krb5();
  krb5_error_code init(const char* user, const char* realm);
  krb5_error_code destroy(const char* name=NULL);
  virtual ~Krb5();
  krb5_error_code get_credentials_by_keytab(const char* keytab=NULL);
  krb5_error_code get_credentials_by_password(char* password);
  OM_uint32 generate_spnego_token(const char* server);
  const char* get_error_message();

  #ifdef NODEGYP
  ///Binding
  static v8::Persistent<v8::FunctionTemplate> tpl;
  //library initialization
  static void Initialize(Handle<Object> target);

  static v8::Handle<v8::Value> getToken(Local<v8::String> property, const v8::AccessorInfo& info);
  static v8::Handle<v8::Value> getErr(Local<v8::String> property, const v8::AccessorInfo& info);

  //async bindings
  static v8::Handle<Value> Init(const v8::Arguments& args);
  static v8::Handle<Value> ByPassword(const v8::Arguments& args);
  static v8::Handle<Value> ByKeyTab(const v8::Arguments& args);
  static v8::Handle<Value> GenToken(const v8::Arguments& args);
  static v8::Handle<Value> Destroy(const v8::Arguments& args);
  //sync bindings
  static v8::Handle<Value> InitSync(const v8::Arguments& args);
  static v8::Handle<Value> ByPasswordSync(const v8::Arguments& args);
  static v8::Handle<Value> ByKeyTabSync(const v8::Arguments& args);
  static v8::Handle<Value> GenTokenSync(const v8::Arguments& args);
  static v8::Handle<Value> DestroySync(const v8::Arguments& args);
  //Constructor Binding
  static v8::Handle<Value> New(const Arguments& args);
  #endif
};
#ifdef NODEGYP

template<typename T> class UvBaton{
public:
  uv_work_t request;
  T* wrapper;
  Persistent<Function> callback;
  UvBaton(const v8::Arguments& args);
  ~UvBaton();
  char ** args;
  unsigned int length;
};

v8::Persistent<FunctionTemplate> Krb5::tpl;

//boilerplate code

extern "C" { // Cause of name mangling in C++, we use extern C here
  static void __init(Handle<Object> target) {
    Krb5::Initialize(target);
  }
  // @see http://github.com/ry/node/blob/v0.2.0/src/node.h#L101
  NODE_MODULE(krb5, __init);
}
#endif //NODEGYP
#endif // KRB_SPNEGO_H

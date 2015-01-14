#include "krb5.h"

static gss_OID_desc _gss_mech_spnego = {6, (void*)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO= &_gss_mech_spnego; //gss_OID == gss_OID_desc*

krb5_error_code Krb5::construct(const char* user, const char* realm){
  if(!realm || !user){
    this->err = -1;
    return this->err;
  }
  int len_realm = strlen(realm);
  int len_user = strlen(user);
  if(!len_realm || !len_user) {
    this->err = -1;
    return this->err;
  }
  this->err = krb5_init_secure_context(&this->context);
  if(this->err) {
    return this->krb5_cleanup(1);
  }
  this->cred = (krb5_creds*)malloc(sizeof(krb5_creds));
  memset(this->cred, 0, sizeof(krb5_creds));
  this->err = krb5_build_principal(this->context, &this->client_principal, len_realm, realm, user, NULL);
  if(this->err) {
    return this->krb5_cleanup(2);
  }
  this->err = krb5_cc_default(this->context, &this->cache);
  if(this->err) {
    return this->krb5_cleanup(3);
  }
  this->err = krb5_cc_initialize(this->context, this->cache, this->client_principal);
  if(this->err) {
    return this->krb5_cleanup(4);
  }
  return this->err;
}

Krb5::Krb5(const char* user, const char* realm) {
  this->keytab=NULL;
  this->spnego_token=NULL;
  if(realm){
    this->realm = (char*)malloc(strlen(realm));
    strcpy(this->realm,realm);
  }
  this->err=0;
  this->construct(user,realm);
}

krb5_error_code Krb5::krb5_cleanup(int level) {
  switch(level) {
  default:
  case 0:
  case 4:
    krb5_cc_close(this->context, this->cache);
  case 3:
    krb5_free_principal(this->context,this->client_principal);
  case 2:
    krb5_free_context(this->context);
  case 1:
    break;
  }
  return this->err;
}

Krb5::~Krb5() {
  this->krb5_cleanup(NO_ERROR);
}

krb5_error_code Krb5::krb5_get_credentials_by_keytab(const char* keytabName) {
  char kt[2048];
  if(!this->err){
    if(keytabName){
      int len = strlen(keytabName);
      if(len) {
        strcpy(kt,"FILE:");
        strcat(kt,realpath(keytabName,NULL));
        this->err = krb5_kt_resolve(this->context, kt, &this->keytab);
      }
      else {
        this->err = krb5_kt_default(this->context,&keytab);
      }
    }
    //SI le path n'est pas précisé, on récupère la keytab par défaut
    else {
      this->err = krb5_kt_default(this->context,&keytab);
    }
    if(this->err) {
      return this->krb5_cleanup(5);
    }
    this->err = krb5_get_init_creds_keytab(context, cred, client_principal, keytab, 0, NULL, NULL);
    if(this->err) {
      return this->krb5_cleanup(6);
    }
    this->krb5_finish_get_cred();
  }
  return this->err;
}


krb5_error_code Krb5::krb5_get_credentials_by_password(const char* password) {
  if(!this->err){
    this->err = krb5_get_init_creds_password(this->context,this->cred,this->client_principal,password, NULL, NULL, 0, NULL, NULL);
    if(this->err) {
      this->krb5_cleanup(6);
      return this->err;
    }
    this->krb5_finish_get_cred();
  }
  return this->err;
}
/*
ROUTINE:
Sauvegarde du token + Initialisation GSS-API + release du token kerberos (inutile, puisqu'importé par GSS-API)
*/
krb5_error_code Krb5::krb5_finish_get_cred() {
  this->err = krb5_verify_init_creds(this->context,this->cred,NULL, NULL, NULL, NULL);
  if(this->err) {
    krb5_free_creds(this->context,this->cred);
    this->krb5_cleanup(6);
    return this->err;
  }
  this->err = krb5_cc_store_cred(this->context, this->cache, this->cred);
  if(this->err) {
    krb5_free_creds(this->context,this->cred);
    this->krb5_cleanup(6);
    return this->err;
  }
  return this->err;
}


OM_uint32 Krb5::import_name(const char* principal, gss_name_t* desired_name) {
  OM_uint32 ret;
  gss_buffer_desc service;
  service.length = strlen(principal);
  service.value = (char*)principal;
  ret=gss_import_name((OM_uint32*)&this->err, &service,GSS_C_NT_HOSTBASED_SERVICE, desired_name);
  return ret;
}
/*
Get the Base64-encoded token
*/
OM_uint32 Krb5::generate_spnego_token(const char* server) {
  char token_buffer[2048];
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
  gss_buffer_desc input_buf,output_buf;

  gss_name_t target_name;
  OM_uint32 err;

  err = import_name(server,&target_name);
  if(err) {
    return this->krb5_cleanup(8);
  }

  err = gss_init_sec_context((OM_uint32*)&this->err,
                             GSS_C_NO_CREDENTIAL,
                             &gss_context,
                             target_name,
                             GSS_MECH_SPNEGO,
                             GSS_C_REPLAY_FLAG | GSS_C_SEQUENCE_FLAG,
                             GSS_C_INDEFINITE,
                             GSS_C_NO_CHANNEL_BINDINGS,
                             &input_buf,
                             NULL,
                             &output_buf,
                             NULL,
                             NULL);

  encode64((char*)output_buf.value,token_buffer,output_buf.length);
  if(this->spnego_token){
    free(this->spnego_token);
  }
  this->spnego_token = (char*) malloc(strlen(token_buffer)+1);
  strcpy(this->spnego_token,token_buffer);
  gss_release_name((OM_uint32*)&this->err, &target_name);
  return 0;
}
#ifdef NODEGYP
/** NODE.JS Binding **/

//A 'non' setter. V8 cannot bind getter without setter. We use this empty setter to avoid data modification
void nonSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
}

void Krb5::Initialize(v8::Handle<v8::Object> target) {
  v8::HandleScope scope; // used by v8 for garbage collection
  // Our constructor
  v8::Local<FunctionTemplate> t = v8::FunctionTemplate::New(Krb5::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(v8::String::NewSymbol("Krb5"));
  // Our getters and setters
  t->InstanceTemplate()->SetAccessor(String::New("token"), Krb5::getToken, nonSetter);
  t->InstanceTemplate()->SetAccessor(String::New("err"), Krb5::getErr, nonSetter);

  // Our methods async
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytab", Krb5::ByKeyTab);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPassword", Krb5::ByPassword);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoToken", Krb5::GenToken);

  //
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytabSync", Krb5::ByKeyTabSync);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPasswordSync", Krb5::ByPasswordSync);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoTokenSync", Krb5::GenTokenSync);


  Krb5::tpl = v8::Persistent<v8::FunctionTemplate>::New(t);

  // Binding our constructor function to the target variable
  target->Set(String::NewSymbol("Krb5"), t->GetFunction());
}

// This is our constructor function. It instantiate a C++ Gtknotify object and returns a Javascript handle to this object.
v8::Handle<Value> Krb5::New(const v8::Arguments& args) {
  HandleScope scope;
  if(args.Length()<2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please use new(user,realm)")));
    return scope.Close(Undefined());
  }
  v8::String::Utf8Value v8user(args[0]);
  v8::String::Utf8Value v8realm(args[1]);

  Krb5* krb = new Krb5(*v8user,*v8realm);
  // Wrap our C++ object as a Javascript object
  krb->Wrap(args.This());
  // Our constructor function returns a Javascript object which is a wrapper for our C++ object,
  // This is the expected behavior when calling a constructor function with the new operator in Javascript.
  return args.This();
}
/** GETTER **/
v8::Handle<v8::Value> Krb5::getToken(Local<v8::String> property, const v8::AccessorInfo& info){
  Krb5* krb = node::ObjectWrap::Unwrap<Krb5>(info.Holder());
  return v8::String::New(krb->spnego_token);
}

v8::Handle<v8::Value> Krb5::getErr(Local<v8::String> property, const v8::AccessorInfo& info){
  Krb5* krb = node::ObjectWrap::Unwrap<Krb5>(info.Holder());
  return v8::String::New(krb5_get_error_message(krb->context,krb->err));
}


/** ASYNC Mode (TODO)  **/
v8::Handle<Value> Krb5::GenToken(const v8::Arguments& args) {
  return Krb5::GenTokenSync(args);
}

v8::Handle<Value> Krb5::ByPassword(const v8::Arguments& args) {
  return Krb5::ByPasswordSync(args);
}

v8::Handle<Value> Krb5::ByKeyTab(const v8::Arguments& args) {
  return Krb5::ByKeyTabSync(args);
}

/** SYNC Mode **/

v8::Handle<Value> Krb5::ByPasswordSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  if(args.Length()>=1){
    v8::String::Utf8Value v8pass(args[0]);
    return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_password(*v8pass)));
  }
  else{
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please add server_principal")));
    return scope.Close(v8::Integer::New(-1));
  }
}

v8::Handle<Value> Krb5::ByKeyTabSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  if(args.Length()>=1){
    v8::String::Utf8Value v8kt(args[0]);
    return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_keytab(*v8kt)));
  }
  return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_keytab(0)));
}

v8::Handle<Value> Krb5::GenTokenSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  if(args.Length()==1){
    v8::String::Utf8Value v8server_name(args[0]);
    return scope.Close(v8::Integer::New(k->generate_spnego_token(*v8server_name)));
  }
  else{
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please add server_principal")));
    return scope.Close(v8::Integer::New(-1));
  }
}

#endif

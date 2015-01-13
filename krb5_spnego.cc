#include "krb5_spnego.h"

static gss_OID_desc _gss_mech_spnego = {6, (void*)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO= &_gss_mech_spnego; //gss_OID == gss_OID_desc*

std::string Krb5Spnego::getSpnegoToken() {
  return this->spnego_token;
}
std::string Krb5Spnego::getRealm() {
  return this->realm;
}

Krb5Spnego::Krb5Spnego(std::string& user, std::string& realm) {
  this->keytab=NULL;
  this->spnego_token="";
  this->realm = realm;
  krb5_error_code err=0;

  if(realm.empty() || user.empty()) {
    krb5_error(0,"params error\n");
  }
  err = krb5_init_secure_context(&this->context);
  if(err) {
    this->krb5_error(1,"init secure context error\n");
  }
  this->cred = (krb5_creds*)malloc(sizeof(krb5_creds));
  memset(this->cred, 0, sizeof(krb5_creds));
  err = krb5_build_principal(this->context, &this->client_principal, realm.length(),realm.c_str(),user.c_str(),NULL);
  if(err) {
    this->krb5_error(2,"build Principal error\n");
  }
  err = krb5_cc_default(this->context, &this->cache);
  if(err) {
    this->krb5_error(3,"cache: no default cache\n");
  }
  err = krb5_cc_initialize(this->context, this->cache, this->client_principal);
  if(err) {
    this->krb5_error(4,"cache: initialization error\n");
  }
}

void Krb5Spnego::krb5_cleanup(int level) {
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
}


void Krb5Spnego::krb5_error(int level, const char* mesg) {
  std::cerr << level << " | " << mesg << std::endl;
  this->krb5_cleanup(level);
}


Krb5Spnego::~Krb5Spnego() {
  this->krb5_cleanup(NO_ERROR);
}

krb5_error_code Krb5Spnego::krb5_get_credentials_by_keytab(std::string& keytabName) {
  krb5_error_code err;
  if(!keytabName.empty()) {
    std::string kt = "FILE:";
    kt+=realpath(keytabName.c_str(),NULL);
    err = krb5_kt_resolve(context, kt.c_str(), &this->keytab);
  }
  //SI le path n'est pas précisé, on récupère la keytab par défaut
  else {
    err = krb5_kt_default(context,&keytab);
  }
  if(err) {
    this->krb5_error(5, "keytab not valid");
    return err;
  }
  err = krb5_get_init_creds_keytab(context, cred, client_principal, keytab, 0, NULL, NULL);
  if(err) {
    this->krb5_error(6,"credential error: using a keytab");
    return err;
  }
  return this->krb5_finish_get_cred();
}


krb5_error_code Krb5Spnego::krb5_get_credentials_by_password(std::string& password) {
  krb5_error_code err;
  if(password.empty()) {
    this->krb5_error(6,"No password\n");
    return -1;
  }
  err = krb5_get_init_creds_password(this->context,this->cred,this->client_principal, (char *)password.c_str(), NULL, NULL, 0, NULL, NULL);
  if(err) {
    this->krb5_error(6,"credential error: using a password");
    return err;
  }
  return this->krb5_finish_get_cred();
}
/*
ROUTINE:
Sauvegarde du token + Initialisation GSS-API + release du token kerberos (inutile, puisqu'importé par GSS-API)
*/
krb5_error_code Krb5Spnego::krb5_finish_get_cred() {
  krb5_error_code err;
  err = krb5_verify_init_creds(this->context,this->cred,NULL, NULL, NULL, NULL);
  if(err) {
    krb5_free_creds(this->context,this->cred);
    this->krb5_error(6,"credential error: verification failed");
    return err;
  }
  err = krb5_cc_store_cred(this->context, this->cache, this->cred);
  if(err) {
    krb5_free_creds(this->context,this->cred);
    this->krb5_error(6,"credentials cache error");
    return err;
  }
  return 0;
}


OM_uint32 Krb5Spnego::import_name(OM_uint32* minor_status,std::string& server, gss_name_t* desired_name) {
  OM_uint32 s,ret;
  gss_buffer_desc service;
  service.value = (char*)malloc(strlen("HTTP")+server.length()+2);
  snprintf((char*)service.value,strlen("HTTP")+server.length()+2,"%s@%s","HTTP",server.c_str());
  service.length = strlen((char*)service.value);
  ret=gss_import_name(minor_status, &service,GSS_C_NT_HOSTBASED_SERVICE, desired_name);
  gss_release_buffer(&s,&service);
  return ret;
}
/*
Get the Base64-encoded token
*/
OM_uint32 Krb5Spnego::generate_spnego_token(std::string& server) {
  char token_buffer[2048];
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
  gss_buffer_desc input_buf,output_buf;

  gss_name_t target_name;
  OM_uint32 minor_status;
  OM_uint32 err=0;

  err=import_name(&minor_status,server,&target_name);

//  err=gss_krb5_import_cred(&minor_status, this->cache,this->client_principal, this->keytab, &this->gss_cred);

  if(err) {
    this->krb5_error(8,"converting kerberos ticket in gss error");
    return err;
  }

  err = gss_init_sec_context(&minor_status,GSS_C_NO_CREDENTIAL,
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
  this->spnego_token = (char*)token_buffer;
  gss_release_name(&minor_status, &target_name);
  return 0;
}

/** NODE.JS Binding **/

//A 'non' setter. V8 cannot bind getter without setter. We use this empty setter to avoid data modification
void nonSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
}

void Krb5Spnego::Initialize(v8::Handle<v8::Object> target) {
  v8::HandleScope scope; // used by v8 for garbage collection
  // Our constructor
  v8::Local<FunctionTemplate> t = v8::FunctionTemplate::New(Krb5Spnego::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(v8::String::NewSymbol("Krb5Spnego"));
  // Our getters and setters
  t->InstanceTemplate()->SetAccessor(String::New("token"), Krb5Spnego::getToken, nonSetter);

  // Our methods async
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytab", Krb5Spnego::ByKeyTab);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPassword", Krb5Spnego::ByPassword);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoToken", Krb5Spnego::GenToken);

  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytabSync", Krb5Spnego::ByKeyTabSync);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPasswordSync", Krb5Spnego::ByPasswordSync);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoTokenSync", Krb5Spnego::GenTokenSync);


  Krb5Spnego::tpl = v8::Persistent<v8::FunctionTemplate>::New(t);

  // Binding our constructor function to the target variable
  target->Set(String::NewSymbol("Krb5Spnego"), t->GetFunction());
}

// new Notification();
// This is our constructor function. It instantiate a C++ Gtknotify object and returns a Javascript handle to this object.
v8::Handle<Value> Krb5Spnego::New(const v8::Arguments& args) {
  HandleScope scope;
  if(args.Length()<2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please use new(user,realm)")));
    return scope.Close(Undefined());
  }
  v8::String::Utf8Value v8user(args[0]);
  v8::String::Utf8Value v8realm(args[1]);

  std::string user= std::string(*v8user);
  std::string realm= std::string(*v8realm);
  Krb5Spnego* krb = new Krb5Spnego(user,realm);
  // Wrap our C++ object as a Javascript object
  krb->Wrap(args.This());
  // Our constructor function returns a Javascript object which is a wrapper for our C++ object,
  // This is the expected behavior when calling a constructor function with the new operator in Javascript.
  return args.This();
}

v8::Handle<v8::Value> Krb5Spnego::getToken(Local<v8::String> property, const v8::AccessorInfo& info){
  Krb5Spnego* krb = node::ObjectWrap::Unwrap<Krb5Spnego>(info.Holder());
  return v8::String::New(krb->spnego_token.c_str());
}

v8::Handle<Value> Krb5Spnego::ByPassword(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string password;
  if(args.Length()==1){
    v8::String::Utf8Value v8pass(args[0]);
    password = std::string(*v8pass);
  }
  else password = "";
  return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_password(password)));
}

v8::Handle<Value> Krb5Spnego::ByKeyTab(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string keytab_name;
  if(args.Length()==1){
    v8::String::Utf8Value v8kt(args[0]);
    keytab_name = std::string(*v8kt);
  }
  else keytab_name = "";
  return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_keytab(keytab_name)));
}

v8::Handle<Value> Krb5Spnego::GenToken(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string server_name;
  if(args.Length()==1){
    v8::String::Utf8Value v8server_name(args[0]);
    server_name = std::string(*v8server_name);
  }
  else server_name = k->getRealm();
  return scope.Close(v8::Integer::New(k->generate_spnego_token(server_name)));
}


v8::Handle<Value> Krb5Spnego::ByPasswordSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string password;
  if(args.Length()==1){
    v8::String::Utf8Value v8pass(args[0]);
    password = std::string(*v8pass);
  }
  else password = "";
  return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_password(password)));
}

v8::Handle<Value> Krb5Spnego::ByKeyTabSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string keytab_name;
  if(args.Length()==1){
    v8::String::Utf8Value v8kt(args[0]);
    keytab_name = std::string(*v8kt);
  }
  else keytab_name = "";
  return scope.Close(v8::Integer::New(k->krb5_get_credentials_by_keytab(keytab_name)));
}

v8::Handle<Value> Krb5Spnego::GenTokenSync(const v8::Arguments& args) {
  v8::HandleScope scope;
  // Extract C++ object reference from "this"
  Krb5Spnego* k = node::ObjectWrap::Unwrap<Krb5Spnego>(args.This());
  std::string server_name;
  if(args.Length()==1){
    v8::String::Utf8Value v8server_name(args[0]);
    server_name = std::string(*v8server_name);
  }
  else server_name = k->getRealm();
  return scope.Close(v8::Integer::New(k->generate_spnego_token(server_name)));
}

#include "krb5.h"
#include <sys/stat.h>

static gss_OID_desc _gss_mech_spnego = {6, (void*)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO= &_gss_mech_spnego; //gss_OID == gss_OID_desc*

bool exists(const char* path){
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

krb5_error_code Krb5::init(const char* user, const char* realm){
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
  this->err = krb5_build_principal(this->context, &this->client_principal, len_realm, realm, user, NULL);
  if(this->err) {
    return this->cleanup(2);
  }
  this->err = krb5_cc_default(this->context, &this->cache);
  if(this->err) {
    return this->cleanup(3);
  }
  if(!exists(krb5_cc_get_name(this->context, this->cache))){
    this->err = krb5_cc_initialize(this->context, this->cache, this->client_principal);
    if(this->err) {
      return this->cleanup(4);
    }
  }
  return this->err;
}

krb5_error_code Krb5::destroy(const char* name){
  krb5_ccache cache;
  if(name){
    this->err = krb5_cc_resolve(this->context, name, &cache);
    if(this->err) return this->err;
  }
  else{
    this->err = krb5_cc_default(this->context, &cache);
    if(this->err) return this->err;
  }
  this->err = krb5_cc_destroy(this->context, cache);
  return this->err;
}

Krb5::Krb5(){
  this->spnego_token=NULL;
  this->err=0;
  this->err = krb5_init_secure_context(&this->context);
  this->cred = (krb5_creds*)malloc(sizeof(krb5_creds));
  memset(this->cred, 0, sizeof(krb5_creds));
  if(this->err) {
    this->cleanup(1);
  }
}

krb5_error_code Krb5::cleanup(int level) {
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
  this->cleanup(NO_ERROR);
}

const char* Krb5::get_error_message(){
  return krb5_get_error_message(this->context, this->err);
}

krb5_error_code Krb5::get_credentials_by_keytab(const char* keytabName) {
  char kt[2048];
  krb5_keytab keytab;
  if(!this->err){
    if(keytabName){
      int len = strlen(keytabName);
      if(len) {
        strcpy(kt,"FILE:");
        strcat(kt,realpath(keytabName,NULL));
        this->err = krb5_kt_resolve(this->context, kt, &keytab);
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
      return this->cleanup(5);
    }
    this->err = krb5_get_init_creds_keytab(context, cred, client_principal, keytab, 0, NULL, NULL);
    if(this->err) {
      return this->cleanup(6);
    }
    this->err = krb5_verify_init_creds(this->context,this->cred,NULL, NULL, NULL, NULL);
    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
    this->err = krb5_cc_store_cred(this->context, this->cache, this->cred);

    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
  }
  return this->err;
}


krb5_error_code Krb5::get_credentials_by_password(const char* password) {
  if(!this->err){
    this->err = krb5_get_init_creds_password(this->context,this->cred,this->client_principal,password, NULL, NULL, 0, NULL, NULL);
    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
    this->err = krb5_cc_store_cred(this->context, this->cache, this->cred);
    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
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
    return this->cleanup(8);
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

template<typename T>
UvBaton<T>::UvBaton(const v8::Arguments& args){
  this->request.data = this;
  this->wrapper = node::ObjectWrap::Unwrap<T>(args.This());
  this->length = args.Length()-1;
  this->callback = Persistent<Function>::New(Handle<Function>::Cast(args[0]));
  this->args = (char**)malloc(this->length*sizeof(char*));
  for(unsigned int i=0; i< this->length; i++){
    v8::String::Utf8Value params(args[i+1]->ToString());
    this->args[i] = (char*)malloc(strlen(*params));
    strcpy(this->args[i],*params);
  }
}
template<typename T>
UvBaton<T>::~UvBaton(){
  for(unsigned int i=0; i<this->length; i++){
    free(this->args[i]);
  }
  free(this->args);
  this->callback.Dispose();
}
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
  NODE_SET_PROTOTYPE_METHOD(t, "init", Krb5::Init);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytab", Krb5::ByKeyTab);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPassword", Krb5::ByPassword);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoToken", Krb5::GenToken);

  //
  NODE_SET_PROTOTYPE_METHOD(t, "initSync", Krb5::InitSync);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByKeytabSync", Krb5::ByKeyTabSync);
  NODE_SET_PROTOTYPE_METHOD(t, "getCredentialsByPasswordSync", Krb5::ByPasswordSync);
  NODE_SET_PROTOTYPE_METHOD(t, "generateSpnegoTokenSync", Krb5::GenTokenSync);


  Krb5::tpl = v8::Persistent<v8::FunctionTemplate>::New(t);

  // Binding our constructor function to the target variable
  target->Set(String::NewSymbol("Krb5"), t->GetFunction());
}

// This is our constructor function. It instantiate a C++ Gtknotify object and returns a Javascript handle to this object.
v8::Handle<Value> Krb5::New(const v8::Arguments& args) {
  Krb5* krb = new Krb5();
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
  return v8::String::New(krb->get_error_message());
}

/** ASYNC Mode (TODO)  **/

void GenTokenAsync(uv_work_t *request){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  baton->wrapper->generate_spnego_token(baton->args[0]);
}

void GenTokenAfter(uv_work_t *request, int status){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  Krb5* k = baton->wrapper;
  if(k->err){
    Handle<Value> argv[2] = {Local<Value>::New(String::New(k->get_error_message())),v8::Null()};
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  }
  else{
    Handle<Value> argv[2] = {v8::Null(),Local<Value>::New(String::New(k->spnego_token))};
    baton->callback->Call(Context::GetCurrent()->Global(), 2, argv);
  }
  delete baton;
}

void InitAsync(uv_work_t *request){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  baton->wrapper->init(baton->args[0], baton->args[1]);
}

void SendErrorAfter(uv_work_t *request, int status){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  Handle<Value> argv[1];
  Krb5* k = baton->wrapper;
  if(k->err){
    argv[0] = Local<Value>::New(String::New(k->get_error_message()));
  }
  else{
    argv[0] = v8::Null();
  }
  baton->callback->Call(Context::GetCurrent()->Global(), 1, argv);
  delete baton;
}


void ByPasswordAsync(uv_work_t *request){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  baton->wrapper->get_credentials_by_password(baton->args[0]);
}

void ByKeytabAsync(uv_work_t *request){
  UvBaton<Krb5>* baton = static_cast<UvBaton<Krb5>*>(request->data);
  if(baton->length>=1){
    baton->wrapper->get_credentials_by_keytab(baton->args[0]);
  }
  else{
    baton->wrapper->get_credentials_by_keytab();
  }
}

v8::Handle<Value> AsyncFunction(const v8::Arguments& args, uv_work_cb asyncFunction, uv_after_work_cb afterFunction){
  UvBaton<Krb5>* baton = new UvBaton<Krb5>(args);
  uv_queue_work(uv_default_loop(), &baton->request, asyncFunction, afterFunction);
  return Undefined();
}


v8::Handle<Value> Krb5::GenToken(const v8::Arguments& args) {
  return AsyncFunction(args, GenTokenAsync, GenTokenAfter);
}

v8::Handle<Value> Krb5::Init(const v8::Arguments& args) {
  return AsyncFunction(args, InitAsync,SendErrorAfter);
}

v8::Handle<Value> Krb5::ByPassword(const v8::Arguments& args) {
  return AsyncFunction(args, ByPasswordAsync,SendErrorAfter);
}

v8::Handle<Value> Krb5::ByKeyTab(const v8::Arguments& args) {
  return AsyncFunction(args, ByKeytabAsync, SendErrorAfter);
}

/** SYNC Mode **/

v8::Handle<Value> Krb5::InitSync(const v8::Arguments& args) {
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  v8::String::Utf8Value v8user(args[0]);
  v8::String::Utf8Value v8realm(args[1]);
  return v8::Integer::New(k->init(*v8user,*v8realm));
}

v8::Handle<Value> Krb5::ByPasswordSync(const v8::Arguments& args) {
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  if(args.Length()>=1){
    v8::String::Utf8Value v8pass(args[0]);
    v8::Handle<Value> ret = v8::Integer::New(k->get_credentials_by_password(*v8pass));
    if(k->err){
      ThrowException(Exception::TypeError(String::New(k->get_error_message())));
    }
    return ret;
  }
  else{
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please add server_principal")));
    return v8::Integer::New(-1);
  }
}

v8::Handle<Value> Krb5::ByKeyTabSync(const v8::Arguments& args) {
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  v8::Handle<Value> ret;
  if(args.Length()>=1){
    v8::String::Utf8Value v8kt(args[0]);
    ret = v8::Integer::New(k->get_credentials_by_keytab(*v8kt));
  }
  else ret = v8::Integer::New(k->get_credentials_by_keytab());
  if(k->err){
    ThrowException(Exception::TypeError(String::New(k->get_error_message())));
  }
  return ret;
}

v8::Handle<Value> Krb5::GenTokenSync(const v8::Arguments& args) {
  // Extract C++ object reference from "this"
  Krb5* k = node::ObjectWrap::Unwrap<Krb5>(args.This());
  if(args.Length()==1){
    v8::String::Utf8Value v8server_name(args[0]);
    v8::Handle<Value> ret = v8::Integer::New(k->generate_spnego_token(*v8server_name));
    if(k->err){
      ThrowException(Exception::TypeError(String::New(k->get_error_message())));
    }
    return ret;
  }
  else{
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments, please add server_principal")));
    return v8::Integer::New(-1);
  }
}

#endif

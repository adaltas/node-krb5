#include "bind.h"
#include "worker.h"
#define INVALID_PARAMETER -5
#define INVALID_PARAMETER_MSG "Invalid parameters"

Nan::Persistent<v8::Function> constructor;

Krb5Wrap::Krb5Wrap(){
  this->k = new Krb5();
  this->k->init_custom_error(INVALID_PARAMETER,INVALID_PARAMETER_MSG);
}

Krb5Wrap::~Krb5Wrap(){
  delete this->k;
}

Krb5* Krb5Wrap::Unwrap(){
  return this->k;
}

//A 'non' setter. V8 cannot bind getter without setter. We use this empty setter to avoid data modification
void nonSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<v8::Value>& info) {
  Nan::ThrowError("Cannot modify properties of Krb5 object");
}

void Krb5Wrap::Initialize(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope; // used by v8 for garbage collection
  // Our constructor
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Krb5").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Our getters and setters
  //tpl->InstanceTemplate()->SetAccessor(Nan::New("token").ToLocalChecked(), getToken, nonSetter);
  //tpl->InstanceTemplate()->SetAccessor(Nan::New("err").ToLocalChecked(), getErr, nonSetter);

  //async
  Nan::SetPrototypeMethod(tpl, "init", Init);
  Nan::SetPrototypeMethod(tpl, "getCredentialsByKeytab", ByKeyTab);
  Nan::SetPrototypeMethod(tpl, "getCredentialsByPassword", ByPassword);
  Nan::SetPrototypeMethod(tpl, "generateSpnegoToken", GenToken);
  Nan::SetPrototypeMethod(tpl, "destroy", Destroy);

  //Sync
  Nan::SetPrototypeMethod(tpl, "initSync", InitSync);
  Nan::SetPrototypeMethod(tpl, "getCredentialsByKeytabSync", ByKeyTabSync);
  Nan::SetPrototypeMethod(tpl, "getCredentialsByPasswordSync", ByPasswordSync);
  Nan::SetPrototypeMethod(tpl, "generateSpnegoTokenSync", GenTokenSync);
  Nan::SetPrototypeMethod(tpl, "destroySync", DestroySync);

  // Binding our constructor function to the target variable
  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Krb5").ToLocalChecked(), tpl->GetFunction());
}

void Krb5Wrap::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Krb5Wrap* krb;
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    krb = new Krb5Wrap();
    krb->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 0;
    v8::Local<v8::Value> argv[argc] = {};
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

/** Async WRAP **/

void GenTokenAsync(Krb5* k, const char* const* args, int length){
  if(length>=1){
    k->generate_spnego_token(args[0]);
  }
  else{
    k->set_error(INVALID_PARAMETER);
  }
}

void InitAsync(Krb5* k, const char* const* args, int length){
  if(length>=2){
    k->init(args[0],args[1]);
  }
  else{
    k->set_error(INVALID_PARAMETER);
  }
}

void ByPasswordAsync(Krb5* k, const char* const* args, int length){
  if(length>=1){
    k->get_credentials_by_password(args[0]);
  }
  else{
    k->set_error(INVALID_PARAMETER);
  }
}

void ByKeytabAsync(Krb5* k, const char* const* args, int length){
  if(length>=1){
    k->get_credentials_by_keytab(args[0]);
  }
  else{
    k->set_error(INVALID_PARAMETER);
  }
}

void DestroyAsync(Krb5* k, const char* const* args, int length){
  if(length>=1){
    k->destroy(args[0]);
  }
  else{
    k->destroy();
  }
}

void Krb5Wrap::GenToken(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::AsyncQueueWorker(Worker::Create(info,GenTokenAsync,true));
}

void Krb5Wrap::Init(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::AsyncQueueWorker(Worker::Create(info,InitAsync));
}

void Krb5Wrap::ByPassword(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::AsyncQueueWorker(Worker::Create(info,ByPasswordAsync));
}

void Krb5Wrap::ByKeyTab(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::AsyncQueueWorker(Worker::Create(info,ByKeytabAsync));
}

void Krb5Wrap::Destroy(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Nan::AsyncQueueWorker(Worker::Create(info,DestroyAsync));
}

/** Sync WRAP **/

void Krb5Wrap::InitSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  if(info.Length()>=1){
    v8::String::Utf8Value v8user(info[0]);
    v8::String::Utf8Value v8realm(info[1]);
    info.GetReturnValue().Set(Nan::New(k->init(*v8user,*v8realm)));
  }
}

void Krb5Wrap::ByPasswordSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // Extract C++ object reference from "this"
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  if(info.Length()>=1){
    v8::String::Utf8Value v8pass(info[0]);
    k->get_credentials_by_password(*v8pass);
    if(k->err){
      Nan::ThrowError(k->get_error_message());
    }
  }
  else{
    k->set_error(INVALID_PARAMETER);
    Nan::ThrowError(k->get_error_message());
  }
}

void Krb5Wrap::ByKeyTabSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // Extract C++ object reference from "this"
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  if(info.Length()>=1){
    v8::String::Utf8Value v8kt(info[0]);
    k->get_credentials_by_keytab(*v8kt);
  }
  else{
    k->get_credentials_by_keytab();
  }
  if(k->err){
    Nan::ThrowError(k->get_error_message());
  }
}

void Krb5Wrap::GenTokenSync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // Extract C++ object reference from "this"
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  if(info.Length()==1){
    v8::String::Utf8Value v8server_name(info[0]);
    k->generate_spnego_token(*v8server_name);
    if(k->err){
      Nan::ThrowError(k->get_error_message());
    }
    else{
      info.GetReturnValue().Set(Nan::New(k->spnego_token).ToLocalChecked());
    }
  }
  else{
    k->set_error(INVALID_PARAMETER);
    Nan::ThrowError(k->get_error_message());
  }
}

void Krb5Wrap::DestroySync(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  // Extract C++ object reference from "this"
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  if(info.Length()==1){
    v8::String::Utf8Value v8cache_name(info[0]);
    k->destroy(*v8cache_name);
  }
  else{
    k->destroy();
  }
  if(k->err){
    Nan::ThrowError(k->get_error_message());
  }
}

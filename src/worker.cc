#include "worker.h"
#include "bind.h"

Worker::Worker(Krb5* k, Nan::Callback* cb, char** args, int args_length, void (*func)(Krb5*, const char* const*, int), bool ret) : Nan::AsyncWorker(cb) {
  this->k = k;
  this->args_length = args_length;
  this->args = args;
  this->func = func;
  this->ret = ret;
}

Worker* Worker::Create(const Nan::FunctionCallbackInfo<v8::Value>& info, void (*func)(Krb5*, const char* const*, int), bool ret) {
  int length = info.Length()-1;
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  Nan::Callback* callback = new Nan::Callback(info[length].As<v8::Function>());
  char** args = (length>0)?new char*[length]:NULL;
  int i;
  for(i=0; i<length; i++){
    v8::String::Utf8Value arg(info[i]);
    args[i] = new char[strlen(*arg)];
    strcpy(args[i], *arg);
  }
  return new Worker(k, callback, args, length, func, ret);
}

Worker::~Worker() {
  int i;
  for(i=0; i<this->args_length; i++){
    free(this->args[i]);
  }
  if(this->args) free(this->args);
}

void Worker::Execute() {
  (*func)(this->k, this->args, this->args_length);
}

void Worker::HandleOKCallback() {
  if(this->ret){
    if(this->k->err){
      v8::Local<v8::Value> argv[] = {Nan::Error(this->k->get_error_message()), Nan::Null()};
      this->callback->Call(2,argv);
    }
    else{
      v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(k->spnego_token).ToLocalChecked()};
      this->callback->Call(2,argv);
    }
  }
  else{
    if(this->k->err){
      v8::Local<v8::Value> argv[] = {Nan::Error(this->k->get_error_message())};
      this->callback->Call(1,argv);
    }
    else{
      v8::Local<v8::Value> argv[] = {Nan::Null()};
      this->callback->Call(1,argv);
    }
  }
}

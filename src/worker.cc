#include "worker.h"
#include "bind.h"

Worker::Worker(Krb5* k, Nan::Callback* cb, v8::Local<v8::Value>* args, int length, void (*func)(Krb5* k, const v8::Local<v8::Value>* args, int length), bool ret) : Nan::AsyncWorker(cb) {
  this->length = length;
  this->k = k;
  this->ret = ret;
  this->func = func;
  this->args = args;
}

Worker* Worker::Create(const Nan::FunctionCallbackInfo<v8::Value>& info, void (*func)(Krb5* k, const v8::Local<v8::Value>* args, int length), bool ret) {
  int length = info.Length()-1;
  Nan::Callback* callback = new Nan::Callback(info[length].As<v8::Function>());
  Krb5* k = ((Krb5Wrap*)Nan::ObjectWrap::Unwrap<Krb5Wrap>(info.This()))->Unwrap();
  v8::Local<v8::Value>* args = new v8::Local<v8::Value>[length];
  int i;
  for(i=0; i<length; i++){
    args[i] = info[i];
  }
  return new Worker(k, callback, args, length, func, ret);
}

Worker::~Worker() {}

void Worker::Execute() {
  (*func)(this->k, this->args, this->length);
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

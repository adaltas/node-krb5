#ifndef WORKER_H
#define WORKER_H

#include <nan.h>
#include "krb5.h"

class Worker : public Nan::AsyncWorker {
public:
  static Worker* Create(const Nan::FunctionCallbackInfo<v8::Value>& info, void (*asyncFunc)(Krb5*, const v8::Local<v8::Value>*, int), bool ret=false);
  ~Worker();
private:
  Krb5* k;
  int length;
  bool ret;
  v8::Local<v8::Value>* args;

  Worker(Krb5*, Nan::Callback*, v8::Local<v8::Value>*, int, void (*)(Krb5*, const v8::Local<v8::Value>*, int), bool);
  virtual void Execute();
  virtual void HandleOKCallback();
  void (*func)(Krb5*, const v8::Local<v8::Value>*, int);
};

#endif

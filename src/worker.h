#ifndef WORKER_H
#define WORKER_H

#include <nan.h>
#include "krb5.h"

class Worker : public Nan::AsyncWorker {
public:
  static Worker* Create(const Nan::FunctionCallbackInfo<v8::Value>&, void (*)(Krb5*, const char* const*, int), bool ret=false);
  ~Worker();
private:
  Krb5* k;
  int args_length;
  bool ret;
  char** args;
  Worker(Krb5*, Nan::Callback*, char**, int, void (*)(Krb5*, const char* const*, int), bool);
  virtual void Execute();
  virtual void HandleOKCallback();
  void (*func)(Krb5*, const char* const*, int);
};

#endif

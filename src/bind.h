#ifndef BIND_H
#define BIND_H
//std libs
#include <nan.h>
#include "krb5.h"

class Krb5Wrap : public Nan::ObjectWrap {
public:
  Krb5Wrap();
  ~Krb5Wrap();
  ///library initialization
  static void Initialize(v8::Local<v8::Object> exports);
  Krb5* Unwrap();

private:
  Krb5* k;

  ///Constructor binding
  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);

  ///async bindings
  static void Init(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void ByPassword(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void ByKeyTab(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GenToken(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Destroy(const Nan::FunctionCallbackInfo<v8::Value>& info);

  ///sync bindings
  static void InitSync(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void ByPasswordSync(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void ByKeyTabSync(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void GenTokenSync(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void DestroySync(const Nan::FunctionCallbackInfo<v8::Value>& info);
};

#endif // BIND_H

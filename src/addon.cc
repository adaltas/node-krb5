#include <nan.h>
#include "bind.h"

void InitAll(v8::Local<v8::Object> exports) {
  Krb5Wrap::Initialize(exports);
}

NODE_MODULE(addon, InitAll)

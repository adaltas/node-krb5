#include "krb5_bind.h"


/**
 * krb5_build_principal
 */
class Worker_krb5_build_principal : public Napi::AsyncWorker {
  public:
    Worker_krb5_build_principal(krb5_context ctx,
                                uint rlen,
                                const char* realm,
                                const char* user,
                                Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx), 
                                     rlen(rlen), 
                                     realm(realm),
                                     user(user) {
    }

  private:
    void Execute() {
      err = krb5_build_principal(context, &princ, rlen, realm, user, NULL);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<krb5_principal>::New(Env(), &princ)
      });
    }

    // In parameters
    krb5_context context;
    uint rlen;
    const char* realm;
    const char* user;

    // Out parameters
    krb5_error_code err;
    krb5_principal princ;
};

Napi::Value _krb5_build_principal(const Napi::CallbackInfo& info) {
  if (info.Length() < 5) {
    throw Napi::TypeError::New(info.Env(),
      "5 arguments expected : context, rlen, realm, user, callback");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  uint rlen = info[1].As<Napi::Number>().Uint32Value();
  const char* realm = info[2].As<Napi::String>().Utf8Value().c_str();
  const char* user = info[3].As<Napi::String>().Utf8Value().c_str();
  Napi::Function callback = info[4].As<Napi::Function>();

  Worker_krb5_build_principal* worker = new Worker_krb5_build_principal(krb_context, 
                                                                        rlen,
                                                                        realm,
                                                                        user,
                                                                        callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_cc_defaut
 */
class Worker_krb5_cc_default: public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_default(krb5_context ctx, Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx) {
    }

  private:
    void Execute() {
      err = krb5_cc_default(context, &ccache);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<krb5_ccache>::New(Env(), &ccache)
      });
    }

    // In parameters
    krb5_context context;

    // Out parameters 
    krb5_error_code err;
    krb5_ccache ccache;
};

Napi::Value _krb5_cc_default(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_krb5_cc_default* worker = new Worker_krb5_cc_default(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_cc_get_name
 */
Napi::Value _krb5_cc_get_name_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  krb5_ccache krb_ccache = *info[1].As<Napi::External<krb5_ccache>>().Data();

  const char* cc_name = krb5_cc_get_name(krb_context, krb_ccache);

  return Napi::String::New(info.Env(), cc_name);
}


/**
 * krb5_cc_initialize_sync
 */
Napi::Value _krb5_cc_initialize_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
     throw Napi::TypeError::New(info.Env(), "3 argument expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  krb5_ccache krb_ccache = *info[1].As<Napi::External<krb5_ccache>>().Data();
  krb5_principal krb_princ = *info[2].As<Napi::External<krb5_principal>>().Data();


  std::cout << "cc_init_sync : " << krb_context << std::endl;
  krb5_error_code err = krb5_cc_initialize(krb_context, krb_ccache, krb_princ);

  return Napi::Number::New(info.Env(), err);
}


/**
 * krb5_cc_initialize
 */
class Worker_krb5_cc_initialize : public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_initialize(krb5_context ctx, 
                              krb5_ccache ccache,
                              krb5_principal princ,
                              Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     krb_ccache(ccache),
                                     krb_princ(princ) {
    }

  private:
    void Execute() {
      std::cout << "cc_init : " << krb_context << std::endl;
      err = krb5_cc_initialize(krb_context, krb_ccache, krb_princ);
      assert(!err);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err)
      });
    }

    // In parameter
    krb5_context krb_context;
    krb5_ccache krb_ccache;
    krb5_principal krb_princ;

    // Out parameter
    krb5_error_code err;
};

Napi::Value _krb5_cc_initialize(const Napi::CallbackInfo& info) {
  if (info.Length() < 4) {
    throw Napi::TypeError::New(info.Env(), "4 arguments expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  krb5_ccache krb_ccache = *info[1].As<Napi::External<krb5_ccache>>().Data();
  krb5_principal krb_princ = *info[2].As<Napi::External<krb5_principal>>().Data();
  Napi::Function callback = info[3].As<Napi::Function>();


  Worker_krb5_cc_initialize* worker = new Worker_krb5_cc_initialize(krb_context, 
                                                                    krb_ccache,
                                                                    krb_princ,
                                                                    callback);
  worker->Queue();
  return info.Env().Undefined();
}

/**
 * krb5_free_context
 */
class Worker_krb5_free_context : public Napi::AsyncWorker {
  public:
    Worker_krb5_free_context(krb5_context ctx, Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx) {
    }

  private:
    void Execute() {
      krb5_free_context(context);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({});
    }

    // In parameters
    krb5_context context;
};

Napi::Value _krb5_free_context(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
    throw Napi::TypeError::New(info.Env(), "2 arguments expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_krb5_free_context* worker = new Worker_krb5_free_context(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_get_default_realm
 */
class Worker_krb5_get_default_realm : public Napi::AsyncWorker {
  public:
    Worker_krb5_get_default_realm(krb5_context ctx, Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx), realm(nullptr) {
    }

  private:
    void Execute() {
      std::cout << "realm_init : " << context << std::endl;
      err = krb5_get_default_realm(context, &realm);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::String::New(Env(), realm)
      });
    }

    // In parameters
    krb5_error_code err;
    krb5_context context;

    // Out parameters 
    char* realm;
};

Napi::Value _krb5_get_default_realm(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }

  krb5_context krb_context = *info[0].As<Napi::External<krb5_context>>().Data();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_krb5_get_default_realm* worker = new Worker_krb5_get_default_realm(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_init_context
 */
class Worker_krb5_init_context : public Napi::AsyncWorker {
  public:
    Worker_krb5_init_context(Napi::Function& callback)
      : Napi::AsyncWorker(callback) {
    }

  private:
    void Execute() {
      err = krb5_init_context(&context);
      std::cout << "init : " << context << std::endl;
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<krb5_context>::New(Env(), &context)
      });
    }

    // Out parameters
    krb5_error_code err;
    krb5_context context;
};

Napi::Value _krb5_init_context(const Napi::CallbackInfo& info) {
  if (info.Length() < 1) {
     throw Napi::TypeError::New(info.Env(), "1 argument expected");
  }
  if (!info[0].IsFunction()) {
    throw Napi::TypeError::New(info.Env(), "Expected callback");
  }

  Napi::Function callback = info[0].As<Napi::Function>();
  Worker_krb5_init_context* worker = new Worker_krb5_init_context(callback);
  worker->Queue();
  return info.Env().Undefined();
}



/**
 * Template worker
 */
/*
class Worker_krb5_template : public Napi::AsyncWorker {
  public:
    Worker_krb5_build_principal(krb5_context ctx,
                                ...,
                                Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx),
                                     ... {
    }

  private:
    void Execute() {
      err = krb5_template(context, ...);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        ...
      });
    }

    // In parameters
    krb5_context context;
    ...

    // Out parameters
    krb5_error_code err;
    ...
};

Napi::Value _krb5_template(const Napi::CallbackInfo& info) {
  if (info.Length() < x) {
    throw Napi::TypeError::New(info.Env(), "x argument expected"
  }

  krb5_context krb_context = *Napi::External<krb5_context>(info.Env(), info[0]).Data();
  Napi::Function callback = info[argc].As<Napi::Function>();

  Worker_krb5_template* worker = new Worker_krb5_template(krb_context, 
                                                          ...
                                                          callback);
  worker->Queue();
  return info.Env().Undefined();
}
*/


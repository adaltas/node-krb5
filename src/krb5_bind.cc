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
        Napi::External<struct krb5_principal_data>::New(Env(), princ)
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

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  uint rlen = info[1].As<Napi::Number>().Uint32Value();
  std::string realm = info[2].As<Napi::String>().Utf8Value();
  std::string user = info[3].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[4].As<Napi::Function>();

  Worker_krb5_build_principal* worker = 
    new Worker_krb5_build_principal(krb_context, 
                                    rlen,
                                    realm.c_str(),
                                    user.c_str(),
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
        Napi::External<struct _krb5_ccache>::New(Env(), ccache)
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

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_krb5_cc_default* worker = new Worker_krb5_cc_default(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_cc_get_name_sync
 */
Napi::Value _krb5_cc_get_name_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();

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

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();
  krb5_principal krb_princ = info[2].As<Napi::External<struct krb5_principal_data>>().Data();

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
      err = krb5_cc_initialize(krb_context, krb_ccache, krb_princ);
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

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();
  krb5_principal krb_princ = info[2].As<Napi::External<struct krb5_principal_data>>().Data();
  Napi::Function callback = info[3].As<Napi::Function>();


  Worker_krb5_cc_initialize* worker = new Worker_krb5_cc_initialize(krb_context, 
                                                                    krb_ccache,
                                                                    krb_princ,
                                                                    callback);
  worker->Queue();
  return info.Env().Undefined();
}

/**
 * krb5_cc_store_cred
 */
class Worker_krb5_cc_store_cred : public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_store_cred(krb5_context ctx, 
                              krb5_ccache ccache,
                              krb5_creds creds,
                              Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     krb_ccache(ccache),
                                     krb_creds(creds) {
    }

  private:
    void Execute() {
      err = krb5_cc_store_cred(krb_context, krb_ccache, &krb_creds);
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
    krb5_creds krb_creds;

    // Out parameter
    krb5_error_code err;
};

Napi::Value _krb5_cc_store_cred(const Napi::CallbackInfo& info) {
  if (info.Length() < 4) {
    throw Napi::TypeError::New(info.Env(), "4 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();
  krb5_creds krb_creds = *info[2].As<Napi::Buffer<krb5_creds>>().Data();
  Napi::Function callback = info[3].As<Napi::Function>();

  Worker_krb5_cc_store_cred* worker = new Worker_krb5_cc_store_cred(krb_context, 
                                                                    krb_ccache,
                                                                    krb_creds,
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

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
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
      err = krb5_get_default_realm(context, &realm);
      //pid_t pid = syscall(__NR_gettid);
      //std::cout << "get)Thread id : " << pid << std::endl;
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::String::New(Env(), realm)
      });
    }

    // In parameters
    krb5_context context;

    // Out parameters 
    krb5_error_code err;
    char* realm;
};

Napi::Value _krb5_get_default_realm(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  Napi::Function callback = info[1].As<Napi::Function>();

  Worker_krb5_get_default_realm* worker = new Worker_krb5_get_default_realm(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}

Napi::Value _krb5_get_default_realm_sync(const Napi::CallbackInfo& info){
  if (info.Length() < 1) {
     throw Napi::TypeError::New(info.Env(), "1 argument expected");
  }
  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();

  char* realm;  
  krb5_error_code err = krb5_get_default_realm(krb_context, &realm);

  return Napi::String::New(info.Env(), realm);
}

/**
 * krb5_get_error_message_sync
 */
Napi::Value _krb5_get_error_message_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }
  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_error_code err_code = info[2].As<Napi::Number>();

  const char* err_msg = krb5_get_error_message(krb_context, err_code);
  Napi::String ret_msg = Napi::String::New(info.Env(), err_msg);
  krb5_free_error_message(krb_context, err_msg);

 return ret_msg;
}


/**
 * krb5_get_init_creds_password
 */

class Worker_krb5_get_init_creds_password : public Napi::AsyncWorker {
  public:
    Worker_krb5_get_init_creds_password(krb5_context ctx, 
                                        krb5_principal princ,
                                        std::string password,
                                        Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     krb_princ(princ),
                                     krb_password(password) {
    }

  private:
    void Execute() {
      std::cout << "Pass : " << krb_password << std::endl;
      err = krb5_get_init_creds_password(krb_context, 
                                         &creds,
                                         krb_princ,
                                         krb_password.c_str(),
                                         nullptr,
                                         nullptr,
                                         0,
                                         NULL,
                                         nullptr);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       
      if (err) {
        std::cout << krb5_get_error_message(krb_context, err) << std::endl;
      }

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::Buffer<krb5_creds>::Copy(Env(), &creds, sizeof(creds))
      });
    }

    // In parameter
    krb5_context krb_context;
    krb5_principal krb_princ;
    std::string krb_password;

    // Out parameter
    krb5_error_code err;
    krb5_creds creds;
};

Napi::Value _krb5_get_init_creds_password(const Napi::CallbackInfo& info) {
  if (info.Length() < 4) {
    throw Napi::TypeError::New(info.Env(), "4 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_principal krb_princ = info[1].As<Napi::External<struct krb5_principal_data>>().Data();
  std::string password = info[2].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[3].As<Napi::Function>();

  std::cout << password.c_str() << std::endl;
  Worker_krb5_get_init_creds_password* worker = 
    new Worker_krb5_get_init_creds_password(krb_context, 
                                            krb_princ, 
                                            password, 
                                            callback);
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
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       
      //pid_t pid = syscall(__NR_gettid);
      //std::cout << "cb)Thread id : " << pid << std::endl;

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<struct _krb5_context>::New(Env(), context)
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

-1765328230L
}
*/


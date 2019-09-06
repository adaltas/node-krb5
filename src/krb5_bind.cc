#include "krb5_bind.h"
#ifdef __MVS__
#include "krb5_zos.h"
#endif


/**
 * krb5_build_principal
 */
class Worker_krb5_build_principal : public Napi::AsyncWorker {
  public:
    Worker_krb5_build_principal(krb5_context ctx,
                                uint rlen,
                                std::string realm,
                                std::string user,
                                Napi::Function& callback)
      : Napi::AsyncWorker(callback), context(ctx), 
                                     rlen(rlen), 
                                     realm(realm),
                                     user(user) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      auto pos = user.find("/");
      if(pos != std::string::npos) {
        err = krb5_build_principal(context,
                                   &princ, 
                                   rlen, 
                                   realm.c_str(), 
                                   user.substr(0, pos).c_str(), 
                                   user.substr(pos+1, user.length()).c_str(),
                                   NULL);
      }
      else {
        err = krb5_build_principal(context, 
                                   &princ, 
                                   rlen, 
                                   realm.c_str(), 
                                   user.c_str(), 
                                   NULL);
      }
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
    std::string realm;
    std::string user;

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
                                    realm,
                                    user,
                                    callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_cc_close
 */
class Worker_krb5_cc_close : public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_close(krb5_context ctx, 
                         krb5_ccache ccache,           
                         Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx),
                                     krb_ccache(ccache) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_cc_close(krb_context, krb_ccache);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err)
      });
    }

    // In parameters
    krb5_context krb_context;
    krb5_ccache krb_ccache;
    
    // Out parameters
    krb5_error_code err;
};

Napi::Value _krb5_cc_close(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
    throw Napi::TypeError::New(info.Env(), "3 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();
  Napi::Function callback = info[2].As<Napi::Function>();

  Worker_krb5_cc_close* worker = new Worker_krb5_cc_close(krb_context,
                                                          krb_ccache,
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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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
 * krb5_cc_destroy
 */
class Worker_krb5_cc_destroy : public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_destroy(krb5_context ctx, 
                           krb5_ccache ccache,           
                           Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx),
                                     krb_ccache(ccache) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_cc_destroy(krb_context, krb_ccache);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err)
      });
    }

    // In parameters
    krb5_context krb_context;
    krb5_ccache krb_ccache;
    
    // Out parameters
    krb5_error_code err;
};

Napi::Value _krb5_cc_destroy(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
    throw Napi::TypeError::New(info.Env(), "3 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_ccache krb_ccache = info[1].As<Napi::External<struct _krb5_ccache>>().Data();
  Napi::Function callback = info[2].As<Napi::Function>();

  Worker_krb5_cc_destroy* worker = new Worker_krb5_cc_destroy(krb_context,
                                                              krb_ccache,
                                                              callback);
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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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
 * krb5_cc_resolve
 */
class Worker_krb5_cc_resolve : public Napi::AsyncWorker {
  public:
    Worker_krb5_cc_resolve(krb5_context ctx, 
                           std::string name,
                           Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     cc_name(name) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_cc_resolve(krb_context, cc_name.c_str(), &ccache);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<struct _krb5_ccache>::New(Env(), ccache)
      });

    }

    // In parameter
    krb5_context krb_context;
    std::string cc_name;
      
    // Out parameter
    krb5_error_code err;
    krb5_ccache ccache;
};

Napi::Value _krb5_cc_resolve(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
    throw Napi::TypeError::New(info.Env(), "3 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  std::string name = info[1].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[2].As<Napi::Function>();

  Worker_krb5_cc_resolve* worker = new Worker_krb5_cc_resolve(krb_context, 
                                                                 name,
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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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

Napi::Value _krb5_free_context_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 1) {
    throw Napi::TypeError::New(info.Env(), "1 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_free_context(krb_context);

  return info.Env().Undefined();
}


/**
 * krb5_free_principal
 */
Napi::Value _krb5_free_principal_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
    throw Napi::TypeError::New(info.Env(), "2 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_principal krb_princ = info[1].As<Napi::External<struct krb5_principal_data>>().Data();
  krb5_free_principal(krb_context, krb_princ);

  return info.Env().Undefined();
}

/**
 * krb5_free_creds
 */
Napi::Value _krb5_free_creds_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
    throw Napi::TypeError::New(info.Env(), "2 arguments expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_creds krb_creds = *info[1].As<Napi::Buffer<krb5_creds>>().Data();
  krb5_free_creds(krb_context, &krb_creds);

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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_get_default_realm(context, &realm);
      //pid_t pid = syscall(__NR_gettid);
      //std::cout << "get)Thread id : " << pid << std::endl;
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::String::New(Env(), realm ? realm : "")
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

  Worker_krb5_get_default_realm* worker = 
    new Worker_krb5_get_default_realm(krb_context, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_get_error_message_sync
 */
Napi::Value _krb5_get_error_message_sync(const Napi::CallbackInfo& info) {
  if (info.Length() < 2) {
     throw Napi::TypeError::New(info.Env(), "2 argument expected");
  }
  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_error_code err_code = info[1].As<Napi::Number>();

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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
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
 * krb5_kt_resolve
 */
class Worker_krb5_kt_resolve : public Napi::AsyncWorker {
  public:
    Worker_krb5_kt_resolve(krb5_context ctx, 
                           std::string kt_name, 
                           Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     kt_name(kt_name) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_kt_resolve(krb_context, kt_name.c_str(), &ktid);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::External<struct _krb5_kt>::New(Env(), ktid)
      });
    }
    

    // In parameters
    krb5_context krb_context;
    std::string kt_name;

    // Out parameters 
    krb5_error_code err;
    krb5_keytab ktid;
};

Napi::Value _krb5_kt_resolve(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
     throw Napi::TypeError::New(info.Env(), "3 argument expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  std::string name = info[1].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[2].As<Napi::Function>();

  Worker_krb5_kt_resolve* worker = 
    new Worker_krb5_kt_resolve(krb_context, name, callback);
  worker->Queue();
  return info.Env().Undefined();
}


/**
 * krb5_get_init_creds_keytab
 * missing param (nullptr): const char* in_tkt_service
 * missing param (nullptr): krb5_get_init_creds_opt * k5_gic_options
 */
class Worker_krb5_get_init_creds_keytab : public Napi::AsyncWorker {
  public:
    Worker_krb5_get_init_creds_keytab(krb5_context ctx, 
                                      krb5_principal princ,
                                      krb5_keytab kt,
                                      krb5_deltat start,
                                      std::string tkt_service,
                                      krb5_get_init_creds_opt * k5_gic_options,
                                      Napi::Function& callback)
      : Napi::AsyncWorker(callback), krb_context(ctx), 
                                     krb_client(princ),
                                     krb_kt(kt),
                                     start_time(start),
                                     krb_tkt_service(tkt_service),
                                     krb_init_creds_opt(k5_gic_options) {
    }

  private:
    void Execute() {
#ifdef __MVS__
      __ae_runmode rm(__AE_ASCII_MODE);
#endif
      err = krb5_get_init_creds_keytab(krb_context,
                                       &creds,
                                       krb_client,
                                       krb_kt,
                                       start_time,
                                       (krb_tkt_service == "") ? 
                                         nullptr : krb_tkt_service.c_str(),
                                       krb_init_creds_opt);
    }

    void OnOK() {
      Napi::HandleScope scope(Env());       

      Callback().Call({
        Napi::Number::New(Env(), err),
        Napi::Buffer<krb5_creds>::Copy(Env(), &creds, sizeof(creds))
      });
    }
    

    // In parameters
    krb5_context krb_context;
    krb5_principal krb_client;
    krb5_keytab krb_kt;
    krb5_deltat start_time;
    std::string krb_tkt_service;
    krb5_get_init_creds_opt* krb_init_creds_opt;

    // Out parameters 
    krb5_error_code err;
    krb5_creds creds;
};

Napi::Value _krb5_get_init_creds_keytab(const Napi::CallbackInfo& info) {
  if (info.Length() < 3) {
     throw Napi::TypeError::New(info.Env(), "5 argument expected");
  }

  krb5_context krb_context = info[0].As<Napi::External<struct _krb5_context>>().Data();
  krb5_principal krb_princ = info[1].As<Napi::External<struct krb5_principal_data>>().Data();
  krb5_keytab krb_kt =  info[2].As<Napi::External<struct _krb5_kt>>().Data();
  krb5_deltat start = info[3].As<Napi::Number>().Int32Value();
  Napi::Function callback = info[4].As<Napi::Function>();

  Worker_krb5_get_init_creds_keytab* worker = 
    new Worker_krb5_get_init_creds_keytab(krb_context,
                                          krb_princ,
                                          krb_kt,
                                          start,
                                          "",   // std::string("") becomes nullptr
                                          nullptr,
                                          callback);
  worker->Queue();
  return info.Env().Undefined();
}

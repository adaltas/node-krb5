#ifndef NODE_KRB_H
#define NODE_KRB_H
//Kerberos libs
#include <krb5.h>
#include <gssapi.h>
#include <gssapi/gssapi_krb5.h>

//local libs
#include "base64.h"

class Krb5 {
public:
  Krb5();
  virtual ~Krb5();
  krb5_context context;
  krb5_error_code err;
  char* spnego_token;
  krb5_error_code init(const char* user, const char* realm);
  krb5_error_code destroy(const char* name=NULL);
  krb5_error_code get_credentials_by_keytab(const char* keytab=NULL);
  krb5_error_code get_credentials_by_password(const char* password);
  OM_uint32 generate_spnego_token(const char* server);
  const char* get_error_message();
  void init_custom_error(krb5_error_code errCode, const char* msg="Unknown Error");
  void set_error(krb5_error_code errCode);

private:
  bool renew;
  krb5_ccache cache;
  krb5_creds* cred;
  krb5_principal client_principal;
  krb5_error_code cleanup(int level=0);
  void krb5_error(int level, char const* mesg);
  OM_uint32 import_name(const char* server, gss_name_t* desired_name);
};

#endif // NODE_KRB_H

#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include "krb5.h"

static gss_OID_desc _gss_mech_spnego = {6, (void*)"\x2b\x06\x01\x05\x05\x02"}; //Spnego OID: 1.3.6.1.5.5.2
static const gss_OID GSS_MECH_SPNEGO= &_gss_mech_spnego; //gss_OID == gss_OID_desc*

bool exists(const char* path){
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

Krb5::Krb5(){
  this->spnego_token=NULL;
  this->err=0;
  this->err = krb5_init_secure_context(&this->context);
  this->cred = (krb5_creds*)malloc(sizeof(krb5_creds));
  memset(this->cred, 0, sizeof(krb5_creds));
  if(this->err) {
    this->cleanup(1);
  }
}

Krb5::~Krb5() {
  this->cleanup();
}

//Kinit function
krb5_error_code Krb5::init(const char* user, const char* realm){
  if(!realm || !user){
    this->err = -1;
    return this->err;
  }
  int len_realm = strlen(realm);
  int len_user = strlen(user);
  if(!len_realm || !len_user) {
    this->err = -1;
    return this->err;
  }
  //Create user principal (user@realm) from user and realm
  this->err = krb5_build_principal(this->context, &this->client_principal, len_realm, realm, user, NULL);
  if(this->err) {
    return this->cleanup(2);
  }
  //Get default credential cache
  this->err = krb5_cc_default(this->context, &this->cache);
  if(this->err) {
    return this->cleanup(3);
  }
  //If default cache does'nt exist, we initialize it
  if(!exists(krb5_cc_get_name(this->context, this->cache))){
    this->err = krb5_cc_initialize(this->context, this->cache, this->client_principal);
    if(this->err) {
      return this->cleanup(4);
    }
  }
  return this->err;
}

krb5_error_code Krb5::destroy(const char* name){
  krb5_ccache cache;
  if(name){
    this->err = krb5_cc_resolve(this->context, name, &cache);
    if(this->err) return this->err;
  }
  else{
    this->err = krb5_cc_default(this->context, &cache);
    if(this->err) return this->err;
  }
  this->err = krb5_cc_destroy(this->context, cache);
  return this->err;
}

krb5_error_code Krb5::cleanup(int level) {
  switch(level) {
  default:
  case 0:
  case 4:
    krb5_cc_close(this->context, this->cache);
  case 3:
    krb5_free_principal(this->context,this->client_principal);
  case 2:
    krb5_free_context(this->context);
  case 1:
    break;
  }
  return this->err;
}

void Krb5::init_custom_error(krb5_error_code errCode, const char* msg){
  krb5_set_error_message(this->context, errCode, msg);
}

void Krb5::set_error(krb5_error_code errCode){
  this->err = errCode;
}

const char* Krb5::get_error_message(){
  return krb5_get_error_message(this->context, this->err);
}

krb5_error_code Krb5::get_credentials_by_keytab(const char* keytabName) {
  char kt[2048];
  krb5_keytab keytab;
  if(!this->err){
    if(keytabName){
      int len = strlen(keytabName);
      if(len) {
        strcpy(kt,"FILE:");
        strcat(kt,keytabName);
        this->err = krb5_kt_resolve(this->context, kt, &keytab);
      }
      else {
        this->err = krb5_kt_default(this->context,&keytab);
      }
    }
    //SI le path n'est pas précisé, on récupère la keytab par défaut
    else {
      this->err = krb5_kt_default(this->context,&keytab);
    }
    if(this->err) {
      return this->cleanup(5);
    }
    this->err = krb5_get_init_creds_keytab(context, cred, client_principal, keytab, 0, NULL, NULL);
    if(this->err) {
      return this->cleanup(6);
    }
    this->err = krb5_verify_init_creds(this->context,this->cred,NULL, NULL, NULL, NULL);
    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
    this->err = krb5_cc_store_cred(this->context, this->cache, this->cred);

    if(this->err) {
      this->cleanup(6);
      return this->err;
    }
  }
  return this->err;
}

krb5_error_code Krb5::get_credentials_by_password(const char* password) {
  if(!this->err){
    this->err = krb5_get_init_creds_password(this->context,this->cred,this->client_principal,password, NULL, NULL, 0, NULL, NULL);
    if(this->err) {
      this->cleanup();
      return this->err;
    }
    this->err = krb5_cc_store_cred(this->context, this->cache, this->cred);
    if(this->err) {
      this->cleanup();
      return this->err;
    }
  }
  return this->err;
}

OM_uint32 Krb5::import_name(const char* principal, gss_name_t* desired_name) {
  OM_uint32 ret;
  gss_buffer_desc service;
  service.length = strlen(principal);
  service.value = (char*)principal;
  ret=gss_import_name((OM_uint32*)&this->err, &service,GSS_C_NT_HOSTBASED_SERVICE, desired_name);
  return ret;
}
/*
Get the Base64-encoded token
*/
OM_uint32 Krb5::generate_spnego_token(const char* server) {
  gss_ctx_id_t gss_context = GSS_C_NO_CONTEXT;
  gss_buffer_desc input_buf,output_buf;
  gss_name_t target_name;
  OM_uint32 gss_err;
  if(this->spnego_token){
    free(this->spnego_token);
  }
  gss_err = import_name(server,&target_name);
  if(gss_err) {
    return this->cleanup();
  }
  gss_err = gss_init_sec_context((OM_uint32*)&this->err,
                  GSS_C_NO_CREDENTIAL,
                  &gss_context,
                  target_name,
                  GSS_MECH_SPNEGO,
                  GSS_C_REPLAY_FLAG | GSS_C_SEQUENCE_FLAG,
                  GSS_C_INDEFINITE,
                  GSS_C_NO_CHANNEL_BINDINGS,
                  &input_buf,
                  NULL,
                  &output_buf,
                  NULL,
                  NULL);
  if(!(GSS_ERROR(gss_err) || this->err)){
    char token_buffer[2048];
    encode64((char*)output_buf.value,token_buffer,output_buf.length);
    this->spnego_token = new char[strlen(token_buffer)+1];
    strcpy(this->spnego_token, token_buffer);
  }
  else{
    if(GSS_ERROR(gss_err) && !this->err){
      char token_buffer[2048];
      OM_uint32 message_context;
      OM_uint32 min_status;
      gss_buffer_desc status_string;
      message_context = 0;
      token_buffer[0] = '\0';
      do {
        gss_display_status(
              &min_status,
              gss_err,
              GSS_C_GSS_CODE,
              GSS_C_NO_OID,
              &message_context,
              &status_string);
        strcat(token_buffer, (char *)status_string.value);
        gss_release_buffer(&min_status, &status_string);
      } while (message_context != 0);
      this->init_custom_error(gss_err,token_buffer);
      this->set_error(gss_err);
    }
    this->spnego_token = NULL;
  }
  return gss_err;
}

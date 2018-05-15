k = require '../../build/Debug/krb5'
fs = require 'fs'


cleanup = (ctx, princ, ccache) ->
  if princ
    k.krb5_free_principal_sync ctx, princ
  if ccache
    k.krb5_cc_close ctx, ccache, (err) ->
      if ctx
        k.krb5_free_context_sync ctx
  else if ctx
    k.krb5_free_context_sync ctx


handle_error = (err, ctx, princ, ccache) ->
  if !err
    return err
  console.log k.krb5_get_error_message_sync(ctx, err)
  cleanup ctx, princ, ccache
  return err


kinit = (user, realm, password) ->
  if !user || !realm
    console.log 'Please specify user and realm for kinit'
    return -1

  k.krb5_init_context (err, ctx) ->
    if err
      return handle_error(err, ctx)

    k.krb5_build_principal ctx, realm.length, realm, user, (err, princ) ->
      if err
        return handle_error(err, ctx, princ)

      k.krb5_cc_default ctx, (err, ccache) ->
        if err
          return handle_error(err, ctx, princ, ccache)
          return err

        cc_name = k.krb5_cc_get_name_sync ctx, ccache
        fs.exists cc_name, (exists) ->
          store_creds_password = () ->
            if password
              k.krb5_get_init_creds_password ctx, princ, password, (err, creds) ->
                if err
                  return handle_error(err, ctx, princ, ccache)
                k.krb5_cc_store_cred ctx, ccache, creds, (err) ->
                  if err
                    return handle_error(err, ctx, princ, ccache)
          
          create_cc = (callback) ->
            if !exists
              k.krb5_cc_initialize ctx, ccache, princ, (err) ->
                if err
                  return handle_error(err, ctx, princ, ccache)
                cc_name = k.krb5_cc_get_name_sync ctx, ccache
                callback()
            else
              callback()

          if password
            create_cc(store_creds_password)
              

module.exports = {
  kinit
}
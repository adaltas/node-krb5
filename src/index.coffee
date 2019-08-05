k = require '../build/Release/krb5'
fs = require 'fs'


cleanup = (ctx, princ, ccache) ->
  k.krb5_free_principal_sync ctx, princ if princ
  
  if ccache
    k.krb5_cc_close ctx, ccache, (err) ->
      k.krb5_free_context_sync ctx if ctx
  else
    k.krb5_free_context_sync ctx if ctx


handle_error = (callback, err, ctx, princ, ccache) ->
  return err unless err
  err = k.krb5_get_error_message_sync(ctx, err)
  cleanup ctx, princ, ccache
  return callback Error err


kinit = (options, callback) ->
  return callback Error 'Please specify principal for kinit' unless options.principal
  return callback Error 'Please specify password or keytab for kinit' unless options.password or options.keytab

  if options.principal.indexOf('@') != -1
    split = options.principal.split('@')
    options.principal = split[0]
    options.realm = split[1]

  do_init = ->
    k.krb5_init_context (err, ctx) ->
      return handle_error callback, err, ctx if err
      do_realm ctx

  do_realm = (ctx) ->
    if !options.realm
      k.krb5_get_default_realm ctx, (err, realm) ->
        return handle_error callback, err, ctx if err
        options.realm = realm
        do_principal ctx
    else
      do_principal ctx

  do_principal = (ctx) ->
    k.krb5_build_principal ctx,
    options.realm.length,
    options.realm,
    options.principal,
    (err, princ) ->
      return handle_error callback, err, ctx if err
      do_ccache ctx, princ

  do_ccache = (ctx, princ) ->
    if options.ccname
      if options.ccname.indexOf(':KEYRING') != -1
        cleanup ctx, princ
        return callback Error 'KEYRING method not supported.'
      k.krb5_cc_resolve ctx, options.ccname, (err, ccache) ->
        return handle_error callback, err, ctx, princ if err
        do_creds ctx, princ, ccache
    else
      k.krb5_cc_default ctx, (err, ccache) ->
        return handle_error callback, err, ctx, princ if err
        do_creds ctx, princ, ccache

  do_creds = (ctx, princ, ccache) ->
    ccname = k.krb5_cc_get_name_sync ctx, ccache
    fs.exists ccname, (exists) ->
      if !exists
        k.krb5_cc_initialize ctx, ccache, princ, (err) ->
          return handle_error callback, err, ctx, princ if err
          if options.password then get_creds_password() else get_creds_keytab()
      else
        if options.password then get_creds_password() else get_creds_keytab()
      
    get_creds_password = ->
      k.krb5_get_init_creds_password ctx, princ, options.password, (err, creds) ->
        return handle_error callback, err, ctx, princ, ccache if err
        store_creds creds

    get_creds_keytab = ->
      k.krb5_kt_resolve ctx, options.keytab, (err, kt) ->
        return handle_error callback, err, ctx, princ, ccache if err
        k.krb5_get_init_creds_keytab ctx, princ, kt, 0, (err, creds) ->
          return handle_error callback, err, ctx, princ, ccache if err
          store_creds creds
            
    store_creds = (creds) ->
      k.krb5_cc_store_cred ctx, ccache, creds, (err) ->
        return handle_error callback, err, ctx, princ, ccache if err
        cleanup ctx, princ, ccache
        callback undefined, ccname
  
  do_init()


kdestroy = (options, callback) ->
  k.krb5_init_context (err, ctx) ->
    return handle_error(callback, err, ctx) if err
    do_ccache(ctx)

  do_ccache = (ctx) ->
    if options.ccname
      k.krb5_cc_resolve ctx, options.ccname, (err, ccache) ->
        return handle_error(callback, err, ctx, null, ccache) if err
        do_destroy ctx, ccache
    else
      k.krb5_cc_default ctx, (err, ccache) ->
        return handle_error(callback, err, ctx, null, ccache) if err
        do_destroy ctx, ccache

  do_destroy = (ctx, ccache) ->
    k.krb5_cc_destroy ctx, ccache, (err) ->
      return handle_error(callback, err, ctx) if err
      callback undefined


spnego = (options, callback) ->
  options.ccname ?= ""
  if options.service_principal
    input_name_type = 'GSS_C_NT_USER_NAME'
    service = options.service_principal
  else if options.service_fqdn or options.hostbased_service
    input_name_type = 'GSS_C_NT_HOSTBASED_SERVICE'
    service = options.service_fqdn or options.hostbased_service
    service = "HTTP@#{service}" unless /.*[@]/.test service
  else return callback Error 'Missing option "service_principal" or "hostbased_service"'

  k.generate_spnego_token service, input_name_type, options.ccname, (err, token) ->
    return callback (if err is "" then undefined else Error err), token


module.exports =
  kinit: (options, callback) ->
    return kinit options, callback if typeof callback is 'function'
    return new Promise (resolve, reject) ->
      kinit options, (err, ccname) ->
        reject err if err
        resolve ccname

  spnego: (options, callback) ->
    return spnego options, callback if typeof callback is 'function'
    return new Promise (resolve, reject) ->
      spnego options, (err, token) ->
        reject err if err
        resolve token

  kdestroy: (options, callback) ->
    options ?= {}
    if typeof options is 'function'
      callback = options
      return kdestroy {}, callback
    else
      return kdestroy options, callback if typeof callback is 'function'
      return new Promise (resolve, reject) ->
        kdestroy options, (err) ->
          reject err if err
          resolve()

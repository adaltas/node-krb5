k = require '../build/Release/krb5'
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


handle_error = (callback, err, ctx, princ, ccache) ->
  if !err
    return err
  err = k.krb5_get_error_message_sync(ctx, err)
  cleanup ctx, princ, ccache
  return callback Error err


kinit = (options, callback) ->
  if !options.principal
    return callback Error 'Please specify principal for kinit'
    
  if !options.password and !options.keytab
    return callback Error 'Please specify password or keytab for kinit'

  if options.principal.indexOf('@') != -1
    split = options.principal.split('@')
    options.principal = split[0]
    options.realm = split[1]

  do_init = ->
    k.krb5_init_context (err, ctx) ->
      return handle_error callback, err if err
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
      process.env.KRB5CCNAME = options.ccname
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
  if options.ccname
    process.env.KRB5CCNAME = options.ccname
  service_principal_or_fqdn = null
  service_principal_or_fqdn ?= options.service_principal
  service_principal_or_fqdn ?= options.service_fqdn
  return callback Error 'Missing property "service_principal" or "service_fqdn"' unless service_principal_or_fqdn
  service_principal_or_fqdn = "HTTP@#{service_principal_or_fqdn}" unless /HTTP[@\/]/.test service_principal_or_fqdn

  k.generate_spnego_token service_principal_or_fqdn, (gss_err, gss_minor, token) ->
    return callback (if gss_err is 0 then undefined else gss_err), token


krb5 = ->
  options = arguments[0]
  queue = []
  err = null

  work = ->
    return unless queue.length
    [name, args] = queue.shift()
    switch name
      when 'kinit'
        kinit options, (err, ccname) ->
          if typeof args[0] is 'function'
            args[0](err, ccname)
          if !err
            work()
      when 'kdestroy'
        kdestroy options, (err) ->
          if typeof args[0] is 'function'
            args[0](err)
          work()
      when 'spnego'
        spnego options, (err, token) ->
          if typeof args[0] is 'function'
            args[0](err, token)
          work()

  process.nextTick work

  kinit: ->
    queue.push ['kinit', arguments]
    return this
  kdestroy: ->
    queue.push ['kdestroy', arguments]
    return this
  spnego: ->
    queue.push ['spnego', arguments]
    return this


krb5.spnego = spnego
krb5.kinit = kinit
krb5.kdestroy = (options, callback) ->
  if typeof options is 'function'
    kdestroy {}, options
  else
    kdestroy options, callback

module.exports = krb5

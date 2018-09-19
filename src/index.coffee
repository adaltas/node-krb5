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


handle_error = (callback, err, ctx, princ, ccache) ->
  if !err
    return err
  err = k.krb5_get_error_message_sync(ctx, err)
  cleanup ctx, princ, ccache
  return callback Error err


kinit = (options, callback) ->
  if !options.username or !options.realm
    return callback Error 'Please specify user and realm for kinit'
    
  if !options.password and !options.keytab
    return callback Error 'Please specify password or keytab for kinit'

  k.krb5_init_context (err, ctx) ->
    return handle_error(callback, err, ctx) if err

    k.krb5_build_principal ctx,
    options.realm.length,
    options.realm,
    options.username,
    (err, princ) ->
      return handle_error(callback, err, ctx, princ) if err

      if options.ccname
        if options.ccname.indexOf(':KEYRING') != -1
          cleanup ctx, princ
          return callback Error 'KEYRING method not supported.'
        k.krb5_cc_resolve ctx, options.ccname, (err, ccache) ->
          return handle_error(callback, err, ctx, princ, ccache) if err
          creds(ccache)
      else
        k.krb5_cc_default ctx, (err, ccache) ->
          return handle_error(callback, err, ctx, princ, ccache) if err
          creds(ccache)

      creds = (ccache) ->
        ccname = k.krb5_cc_get_name_sync ctx, ccache
        fs.exists ccname, (exists) ->
          create_cc = (create_cc_callback) ->
            if !exists
              k.krb5_cc_initialize ctx, ccache, princ, (err) ->
                return handle_error(callback, err, ctx, princ, ccache) if err
                create_cc_callback()
            else
              create_cc_callback()

          get_creds_password = () ->
            k.krb5_get_init_creds_password ctx, princ, options.password, (err, creds) ->
              return handle_error(callback, err, ctx, princ, ccache) if err
              store_creds creds

          get_creds_keytab = () ->
            k.krb5_kt_resolve ctx, options.keytab, (err, kt) ->
              return handle_error(callback, err, ctx, princ, ccache) if err
              k.krb5_get_init_creds_keytab ctx, princ, kt, 0, (err, creds) ->
                return handle_error(callback, err, ctx, princ, ccache) if err
                store_creds creds
                  
          store_creds = (creds) ->
            k.krb5_cc_store_cred ctx, ccache, creds, (err) ->
              return handle_error(callback, err, ctx, princ, ccache) if err
              ccname = k.krb5_cc_get_name_sync ctx, ccache
              callback undefined, { ccname: ccname }

          create_cc if options.password then get_creds_password else get_creds_keytab
          return

spnego = (options, callback) ->
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
        kinit options, (err, ret) ->
          if typeof args[0] is 'function'
            args[0](err, ret)
          if !err
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
  spnego: ->
    queue.push ['spnego', arguments]
    return this

krb5.kinit = kinit
krb5.spnego = spnego

module.exports = krb5


# TODO



# err undefined et non 0
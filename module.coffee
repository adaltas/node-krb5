krb = require './build/Debug/krb5'

krb.krb5_init_context (err, ctx) ->
  if err
    console.log 'Error: Krb5 init context failed.'
    return
  console.log 'Krb5 context initilizied'

  krb.krb5_get_default_realm ctx, (err, realm) ->
    if err
      console.log 'Error: cannot get default realm'
      return
    console.log 'Default realm: ', realm

    krb.krb5_build_principal ctx, realm.length, realm, 'user', (err, princ) ->
      if err
        console.log 'Error: connot build principal'
        return
      console.log 'Principal build'

      krb.krb5_cc_default ctx, (err, ccache) ->
        if err
          console.log 'Error: cannot retrieve default ccache'
          return

        krb.krb5_cc_initialize ctx, ccache, princ, (err) ->
          if err
            console.log 'Cannot initialize credential cache'
            return
          ccache_name = krb.krb5_cc_get_name_sync ctx, ccache
          console.log ccache_name

          krb.krb5_free_context ctx, ->


# To generate js file:
#   coffee -cb module.coffee
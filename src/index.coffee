krb5 = require './index.node'

module.exports =
  kinit: (options, callback) ->
    return krb5.kinit options unless callback
    krb5.kinit options
      .then (ccname) -> callback undefined, ccname
      .catch (err) -> callback err
    return

  spnego: (options, callback) ->
    return krb5.spnego options  unless callback
    krb5.spnego options
      .then (token) -> callback undefined, token
      .catch (err) -> callback err
    return

  kdestroy: (options, callback) ->
    options ?= {}
    return krb5.kdestroy options unless typeof options is 'function' or callback
    if typeof options is 'function'
      callback = options
      options = {}

    krb5.kdestroy options
      .then () -> callback()
      .catch (err) -> callback err
    return

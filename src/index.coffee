
fs = require 'fs'
krb5 = require('bindings')('krb5')

module.exports = (options, callback) ->
  k = new Krb5 options
  if callback
    k.kinit (err) -> callback err, k
  k

module.exports.spnego = (options, callback) ->
  return callback new Error 'Params: Not an object' unless typeof options is 'object'
  return callback new Error 'Params: Please set principal property' unless options.principal?
  return callback new Error 'Params: principal is unvalid, please use ID@REALM' unless options.principal.match /[A-Za-z0-9_\-\/]*@[A-Za-z0-9_\-\.]*/
  return callback new Error 'Params: please set service_principal property' unless options.service_principal?
  return callback new Error 'Params: service_principal is unvalid, please use ID@REALM' unless options.service_principal.match /[A-Za-z0-9_\-\/]*@[A-Za-z0-9_\-\.]*/
  k = new Krb5 options
  k.kinit (err) ->
    return callback err if err
    k.token options.service_principal, callback
  k

###

Options includes:

*   `principal`   
*   `password`   
*   `keytab`   
*   `service_principal`   
*   `service_fqdn`   
*   `ccname`   

###

Krb5 = module.exports.Krb5 = (@options) ->
  @k = new krb5.Krb5
  @
Krb5::kinitSync = () ->
  if @options.ccname?.indexOf(':') is -1
    stat = fs.statSync @options.ccname
    if stat.isFile()
      @options.ccname =+ "FILE:"
    else if stat.isDirectory()
      @options.ccname =+ "DIR:"
    else return throw Error 'Invalid Option "ccname"'
  throw Error 'principal not set' unless @options.principal?
  [user, realm] = @options.principal.split '@'
  if @options.ccname?
    process.env.KRB5CCNAME = @options.ccname
    @k.initSync user, realm, @options.ccname.split(':')[1]
  else
    @k.initSync user, realm
  if @options.password?
    @k.getCredentialsByPasswordSync @options.password
  else if @options.keytab?
    @k.getCredentialsByKeytabSync @options.keytab
  else
    @k.getCredentialsByKeytabSync
Krb5::kinit = (callback) ->
  return callback Error 'Missing Property "principal"' unless @options.principal?
  do_ccname = =>
    return do_kinit() if not @options.ccname or @options.ccname.indexOf(':') isnt -1
    fs.stat @options.ccname, (err, stat) ->
      if stat.isFile()
        @options.ccname =+ "FILE:"
      else if stat.isDirectory()
        @options.ccname =+ "DIR:"
      else return callback Error 'Invalid Option "ccname"'
      process.env.KRB5CCNAME = @options.ccname if @options.ccname
      do_kinit()
  do_kinit = =>
    [user, realm] = @options.principal.split '@'
    @k.init user, realm, ((err) -> if err then callback err else do_credential())
  do_credential = =>
    if @options.password?
      method = 'getCredentialsByPassword'
      param = @options.password
    else if @options.keytab?
      method = 'getCredentialsByKeytab'
      param = @options.keytab
    else
      callback Error 'Invalid arguments'
    @k[method] param, callback
  do_ccname()
Krb5::kdestroySync = (cache) ->
  return if cache? then @k.destroySync cache else @k.destroySync()
Krb5::kdestroy = (cache, callback) ->
  if typeof cache is 'function'
    callback = cache
    cache = null
  if cache? then @k.destroy cache, callback else @k.destroy callback
Krb5::tokenSync = (service_principal_or_fqdn) ->
  service_principal_or_fqdn ?= @options.service_principal
  service_principal_or_fqdn ?= @options.service_fqdn
  throw Error 'Missing property "service_principal" or "service_fqdn"' unless service_principal_or_fqdn
  service_principal_or_fqdn = "HTTP@#{service_principal_or_fqdn}" unless /HTTP[@\/]/.test service_principal_or_fqdn
  @k.generateSpnegoTokenSync service_principal_or_fqdn
Krb5::token = (service_principal_or_fqdn, callback) ->
  if arguments.length is 1
    callback = service_principal_or_fqdn
    service_principal_or_fqdn = null
  ts = Date.now()
  if @last_token_ts is ts
    return setTimeout =>
      @token service_principal_or_fqdn, callback
    , 1
  @last_token_ts = ts
  service_principal_or_fqdn ?= @options.service_principal
  service_principal_or_fqdn ?= @options.service_fqdn
  return callback Error 'Missing property "service_principal" or "service_fqdn"' unless service_principal_or_fqdn
  service_principal_or_fqdn = "HTTP@#{service_principal_or_fqdn}" unless /HTTP[@\/]/.test service_principal_or_fqdn
  @k.generateSpnegoToken service_principal_or_fqdn, callback


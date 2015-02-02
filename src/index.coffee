krb5 = require '../build/Release/krb5'

module.exports = (obj, next) ->
  return next new Error 'Params: Not an object' unless typeof obj is 'object'
  return next new Error 'Params: Please set client_principal property' unless obj.client_principal?
  return next new Error 'Params: client_principal is unvalid, please use ID@REALM' unless obj.client_principal.match /[A-Za-z0-9_\-\/]*@[A-Za-z0-9_\-\.]*/
  return next new Error 'Params: please set service_principal property' unless obj.service_principal?
  return next new Error 'Params: service_principal is unvalid, please use ID@REALM' unless obj.service_principal.match /[A-Za-z0-9_\-\/]*@[A-Za-z0-9_\-\.]*/
  k = new krb5.Krb5
  [user, realm] = obj.client_principal.split '@'
  if obj.cc_file?
    process.env.KRB5CCNAME = "FILE:#{obj.cc_file}"
  else if obj.cc_dir?
    process.env.KRB5CCNAME = "DIR:#{obj.cc_dir}"
  k.initSync user, realm
  if obj.password?
    k.getCredentialsByPasswordSync obj.password
  else
    k.getCredentialsByKeytabSync obj.keytab
  toto = k.generateSpnegoTokenSync obj.service_principal
  next(new Error(k.err), k.token)

module.exports.Krb5 = (obj) ->
  k = new krb5.Krb5
  self = this
  @kinitSync = (obj) ->
    if obj?
      @client_principal = obj.client_principal if obj.client_principal?
      @password = obj.password if obj.password?
      @keytab = obj.keytab if obj.keytab?
      if obj.cc_file?
        @cc_path = obj.cc_file
        @cc_type = "FILE"
      else if obj.cc_dir?
        @cc_path = obj.cc_dir
        @cc_type = "DIR"
    throw new Error 'client_principal not set' unless self.client_principal?
    [user, realm] = @client_principal.split '@'
    if @cc_type? and @cc_path?
      process.env.KRB5CCNAME = "#{@cc_type}:#{@cc_path}"
      k.initSync user, realm, @cc_path
    else
      k.initSync user, realm
    if self.password?
      k.getCredentialsByPasswordSync @password
    else if self.keytab?
      k.getCredentialsByKeytabSync @keytab
    else
      k.getCredentialsByKeytabSync
  
  @kinit = (obj, next) ->
    if typeof obj is 'function'
      next = obj
      obj = null
    if obj?
      @client_principal = obj.client_principal if obj.client_principal?
      @password = obj.password if obj.password?
      @keytab = obj.keytab if obj.keytab?
      if obj.cc_file?
        @cc_path = obj.cc_file
        @cc_type = "FILE"
      else if obj.cc_dir?
        @cc_path = obj.cc_dir
        @cc_type = "DIR"
    return next new Error 'client_principal not set' unless @client_principal?
    [user, realm] = @client_principal.split '@'
    __next = (err) -> 
      if self.password?
        k.getCredentialsByPassword next, self.password
      else if self.keytab?
        k.getCredentialsByKeytab next, self.keytab
      else
        k.getCredentialsByKeytabSync next
    if self.cc_type? and self.cc_path?
      process.env.KRB5CCNAME = "#{self.cc_type}:#{self.cc_path}"
    k.init __next, user, realm
  @kdestroySync = (cache) ->
    return if cache? then k.destroySync cache else k.destroySync()
  @kdestroy = (cache, next) ->
    if typeof cache is 'function'
      next = cache
      cache = null
    if cache? then k.destroy next, cache else k.destroy next
  @tokenSync = (host) ->
    if host? then k.generateSpnegoTokenSync host
    else if @service_principal? then k.generateSpnegoTokenSync @service_principal
    else throw new Error 'please specify host'
    return k.token
  @token = (host, next) ->
    if host? and not next? and typeof host is 'function'
      next = host
      host = null
    if host? then k.generateSpnegoToken next, host
    else if @service_principal? then k.generateSpnegoToken next, @service_principal
    else next (new Error 'please specify host')

  throw new Error 'Parameter error' if obj and typeof obj isnt 'object'
  if obj?
    @client_principal = obj.client_principal if obj.client_principal?
    @password = obj.password if obj.password?
    @keytab = obj.keytab if obj.keytab?
    @service_principal = obj.service_principal if obj.service_principal?
    if obj.cc_file?
      @cc_path = obj.cc_file
      @cc_type = "FILE"
    else if obj.cc_dir?
      @cc_path = obj.cc_dir
      @cc_type = "DIR"

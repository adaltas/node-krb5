krb5 = require './build/Release/krb5'

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
    k.initSync user, realm, obj.cc_file
  else if obj.cc_dir?
    process.env.KRB5CCNAME = "DIR:#{obj.cc_dir}"
    k.initSync user, realm, obj.cc_dir
  else
    k.initSync user, realm
  if obj.user_password
    k.getCredentialsByPasswordSync obj.user_password
  else
    k.getCredentialsByKeytabSync obj.user_keytab
  toto = k.generateSpnegoTokenSync obj.service_principal
  next(new Error(k.err), k.token)

module.exports.Krb5 = (obj) ->
  self = this
  k = new krb5.Krb5
  @kinitSync = (obj) ->
    if obj?
      self.client_principal = obj.client_principal if obj.client_principal?
      self.password = obj.password if obj.password?
      self.keytab = obj.keytab if obj.keytab?
      if obj.cc_file?
        self.cc_path = obj.cc_file
        self.cc_type = "FILE"
      else if obj.cc_dir?
        self.cc_path = obj.cc_dir
        self.cc_type = "DIR"
    throw new Error 'client_principal not set' unless self.client_principal?
    [user, realm] = self.client_principal.split '@'
    if self.cc_type? and @cc_path?
      process.env.KRB5CCNAME = "#{@cc_type}:#{@cc_path}"
      k.initSync user, realm, @cc_path
    else
      k.initSync user, realm
    if self.password?
      k.getCredentialsByPasswordSync self.password
    else if self.keytab?
      k.getCredentialsByKeytabSync self.keytab
    else
      k.getCredentialsByKeytabSync
  
  @kinit = (obj, next) ->
    if typeof obj is 'function'
      next = obj
      obj = null
    if obj?
      self.client_principal = obj.client_principal if obj.client_principal?
      self.password = obj.password if obj.password?
      self.keytab = obj.keytab if obj.keytab?
      if obj.cc_file?
        self.cc_path = obj.cc_file
        self.cc_type = "FILE"
      else if obj.cc_dir?
        self.cc_path = obj.cc_dir
        self.cc_type = "DIR"
    return next new Error 'client_principal not set' unless self.client_principal?
    [user, realm] = self.client_principal.split '@'
    __next = (err) -> 
      if self.password?
        k.getCredentialsByPassword next, self.password
      else if self.keytab?
        k.getCredentialsByKeytab next, self.keytab
      else
        k.getCredentialsByKeytabSync next
    if self.cc_type? and self.cc_path?
      process.env.KRB5CCNAME = "#{@cc_type}:#{@cc_path}"
      k.init __next, user, realm, cc_path
    else
      k.init __next, user, realm
    
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

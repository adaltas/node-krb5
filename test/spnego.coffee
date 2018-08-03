k = require '../lib/krb5'


realm = 'KRB.LOCAL'

k.kinit {
  username: 'hbase/m01.krb.local'
  keytab: 'FILE:/tmp/hbase.service.keytab'
  realm: realm
}, (err, { cc_path }) ->
  console.log err
  console.log cc_path
  k.spnego {
    service_fqdn: 'm01.krb.local'
  }, (gss_err, gss_minor, token) ->
    console.log 'GSS Status', gss_err, '(', gss_minor, ')'
    console.log 'Token: ', token


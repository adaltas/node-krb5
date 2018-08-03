var k = require('../lib/krb5');

k.kinit({
  username: 'name',
  realm: 'KRB.LOCAL',
  keytab: '/etc/security/keytabs/me.keytab'
}, function (err, arg) {
  if (!err) {
    console.log('Credentials stored in ', arg.cc_path)
  }

  k.spnego({
    service_fqdn: 'm01.krb.local'
  }, function (err, token) {
    if (!err) {
      console.log('SPNEGO token : ', token)
    }
  })
})

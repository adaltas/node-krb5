/**
 * Sample
 * Retrieve SPNEGO token to access a secured HBase REST API
 */
var krb5 = require('../lib/');

krb5({
  username: 'hbase/m01.krb.local',
  keytab: '/tmp/hbase.service.keytab',
  realm: 'KRB.LOCAL',
  service_fqdn: 'm01.krb.local'
}).kinit(function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
  }
}).spnego(function (err, token) {
  if (err) {
    console.log(err)
  } else {
    console.log('SPNEGO token :', token)
  }
});

// Using static functions

krb5.kinit({
  username: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL'
}, function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
    krb5.spnego({
      service_fqdn: 'm01.krb.local'
    }, function (err, token) {
      if (err) {
        console.log(err)
      } else {
        console.log('SPNEGO token :', token)
      }
    })
  }
});



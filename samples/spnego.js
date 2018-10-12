var krb5 = require('../lib/');


krb5.kinit({
  principal: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL'
}, function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)

    krb5.spnego({
      service_fqdn: 'rest.krb.local'
    }, function (err, token) {
      if (err) {
        console.log(err)
      } else {
        console.log('SPNEGO token :', token)
      }
    })
  }
});

krb5.kinit({
  principal: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL'
}).then(function (ccname) {
  console.log('Credentials saved in', ccname)
  return krb5.spnego({
    hostbased_service: 'HTTP@rest.krb.local'
  })
}).then(function (token) {
  console.log('SPNEGO token :', token)
}).catch(function (err) {
  console.log(err)
})

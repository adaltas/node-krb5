var krb5 = require('../lib/');


krb5.kinit({
  principal: 'admin@KRB.LOCAL',
  password: 'adm1n_p4ssw0rd',
  ccname: '/tmp/customcc'
}, function (ccname, err) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)

    krb5.kdestroy({ ccname: '/tmp/customcc' }, function (err) {
      if (err) {
        console.log(err)
      } else {
        console.log('Credential cache destroyed.')
      }
    })
  }
});



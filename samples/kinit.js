var krb5 = require('../lib/');

krb5({
  username: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL',
}).kinit(function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
  }
});

krb5.kinit({
  username: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL',
  ccname: '/tmp/customcc'     //optionnal
}, function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
  }
});


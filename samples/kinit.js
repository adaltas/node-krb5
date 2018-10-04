var krb5 = require('../lib/');


krb5.kinit({
  principal: 'admin',
  password: 'adm1n_p4ssw0rd',
  realm: 'KRB.LOCAL',
  ccname: '/tmp/customcc'
}, function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
  }
});


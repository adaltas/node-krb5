k = require '../lib/krb5'

console.log 'kinit test'
k.kinit {
  username: 'admin'
  password: 'adm1n_p4ssw0rd'
  realm: 'KRB.LOCAL'
}, (err, { cc_path }) ->
  console.log err?.message or cc_path
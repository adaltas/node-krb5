k = require '../lib/krb5'

###
mocha test/*.coffee

TODO : docker with kerberos KDC for standalone tests.
At the moment, tests done with FreeIPA KDC in VM ldap01.krb.local
###

realm = 'KRB.LOCAL'

describe 'kinit', ->
  it 'should return credential cache path (password provided)', (done) ->
    k.kinit {
      username: 'admin'
      password: 'adm1n_p4ssw0rd'
      realm: realm
    }, (err, { cc_path }) ->
      err.should.equal(0)
      cc_path.should.startWith('/tmp')
      done()


  it 'should return credential cache path (keytab provided)', (done) ->
    k.kinit {
      username: 'admin'
      realm: realm
      keytab: './test/test.keytab'
    }, (err, { cc_path }) ->
      err.should.equal(0)
      cc_path.should.startWith('/tmp')
      done()

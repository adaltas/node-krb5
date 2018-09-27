krb5 = require '../lib/'

describe 'kinit', ->

  describe 'method with callback', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .kinit (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done err

    it 'returns default credential cache path (password provided using default realm)', (done) ->
      krb5
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
      .kinit (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done err
 
    it 'returns default credential cache path (password provided using realm in principal)', (done) ->
      krb5
        principal: 'admin@KRB.LOCAL'
        password: 'adm1n_p4ssw0rd'
        realm: 'to_override'
      .kinit (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done err

    it 'returns default credential cache path (keytab provided)', (done) ->
      krb5
        principal: 'hbase/hbase.krb.local'
        keytab: '/tmp/krb5_test/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      .kinit (err, ccname) ->
        console.log err
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

    it 'returns given credential cache path (keytab provided)', (done) ->
      krb5
        principal: 'hbase/hbase.krb.local'
        keytab: '/tmp/krb5_test/hbase.service.keytab'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      .kinit (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.be.equal '/tmp/customcc'
        done()

###
  describe 'method with promise', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .kinit()
      .catch done
      .then (ccname) ->
        ccname.should.startWith('/tmp')
        done()
###

describe 'kinit', ->

  describe 'function with callback', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

    it 'returns default credential cache path (keytab provided)', (done) ->
      krb5.kinit
        principal: 'hbase/hbase.krb.local'
        keytab: '/tmp/krb5_test/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

    it 'returns given credential cache path (keytab provided)', (done) ->
      krb5.kinit
        principal: 'hbase/hbase.krb.local'
        keytab: '/tmp/krb5_test/hbase.service.keytab'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      , (err, ccname) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

###
  describe 'function with promise', ->
    it 'returns default credential cache path (password provided)', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .catch done
      .then (ccname) ->
        ccname.should.startWith('/tmp')
        done()
###


krb5 = require '../lib/krb5'


describe 'kinit', ->

  describe 'method with callback', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5
        username: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .kinit (err, ret) ->
        (err is undefined).should.be.true()
        ret.ccname.should.startWith('/tmp')
        done err
    
    it 'returns default credential cache path (keytab provided)', (done) ->
      krb5
        username: 'hbase/m01.krb.local'
        keytab: '/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      .kinit (err, { ccname }) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

    it 'returns given credential cache path (keytab provided)', (done) ->
      krb5
        username: 'hbase/m01.krb.local'
        keytab: '/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      .kinit (err, ret) ->
        (err is undefined).should.be.true()
        ret.ccname.should.be.equal '/tmp/customcc'
        done()

  describe 'method with promise', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5
        username: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .kinit()
      .catch done
      .then ({ ccname }) ->
        ccname.should.startWith('/tmp')
        done()

describe 'kinit', ->

  describe 'function with callback', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5.kinit
        username: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      , (err, { ccname }) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()

    it 'returns default credential cache path (keytab provided)', (done) ->
      krb5.kinit
        username: 'hbase/m01.krb.local'
        keytab: '/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, { ccname }) ->
        (err is undefined).should.be.true()
        ccname.should.startWith('/tmp')
        done()
  
  describe 'function with promise', ->
    it 'returns default credential cache path (password provided)', (done) ->
      krb5.kinit
        username: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .catch done
      .then ({ ccname }) ->
        ccname.should.startWith('/tmp')
        done()


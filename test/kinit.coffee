krb5 = require '../lib/'

describe 'kinit', ->

  describe 'function with callback', ->

    it 'returns default credential cache path (password provided)', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        return done err if err
        ccname.should.startWith('/tmp')
        done()

    it 'returns default credential cache path (password provided using default realm)', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
      , (err, ccname) ->
        return done err if err
        ccname.should.startWith('/tmp')
        done err

    it 'returns default credential cache path (password provided using realm in principal)', (done) ->
      krb5.kinit
        principal: 'admin@KRB.LOCAL'
        password: 'adm1n_p4ssw0rd'
        realm: 'to_override'
      , (err, ccname) ->
        return done err if err
        ccname.should.startWith('/tmp')
        done err

    it 'returns default credential cache path (keytab provided)', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: '/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        return done err if err
        ccname.should.startWith('/tmp')
        done()

    it 'returns given credential cache path (keytab provided)', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: '/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      , (err, ccname) ->
        return done err if err
        ccname.should.be.eql('/tmp/customcc')
        done()

  describe 'function with promise', ->

    it 'returns default credential cache path (password provided)', ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .then (ccname) ->
        ccname.should.startWith('/tmp')

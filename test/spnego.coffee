krb5 = require '../lib/krb5'

describe 'spnego', ->

  describe 'method with callback', ->

    it 'returns SPNEGO token', (done) ->
      krb5
        username: 'hbase/m01.krb.local'
        keytab: '/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
        service_fqdn: 'm01.krb.local'
      .kinit (err, ret) ->
      .spnego (gss_err, token) ->
          (gss_err is undefined).should.be.true()
          token.should.be.String()
          done()

describe 'spnego', ->

  describe 'function with callback', ->

    it 'returns SPNEGO token', (done) ->
      krb5.kinit
        username: 'hbase/m01.krb.local'
        keytab: 'FILE:/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ret) ->
        krb5.spnego
          service_fqdn: 'm01.krb.local'
        , (err, token) ->
          (err is undefined).should.be.true()
          token.should.be.String()
          done()

  describe 'function with promise', ->
    
    it 'returns SPNEGO token', (done) ->
      krb5.kinit
        username: 'hbase/m01.krb.local'
        keytab: 'FILE:/tmp/hbase.service.keytab'
        realm: 'KRB.LOCAL'
      .catch done
      .then ({ cc_patch }) ->
        krb5.spnego
          service_fqdn: 'm01.krb.local'
        .catch done
        .then (token) ->
          token.should.be.String()
          done()


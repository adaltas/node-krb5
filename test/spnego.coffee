krb5 = require '../lib/'
http = require 'http'

describe 'spnego', ->

###
  describe 'method with callback', ->

    it 'returns SPNEGO token', (done) ->
      krb5
        principal: 'rest/rest.krb.local'
        keytab: '/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
        service_fqdn: 'rest.krb.local'
      .kinit (err, ccname) ->
      .spnego (gss_err, token) ->
          (gss_err is undefined).should.be.true()
          token.should.be.String()
          done()
###

  describe 'function with callback', ->
    
    it 'checks that server requires authentication', (done) ->
      http.request
        hostname: 'rest.krb.local'
        port: 8080
        path: '/'
        method: 'GET'
      , (res) ->
        res.statusCode.should.be.eql(401)
        done()
      .end()

    it 'authenticates to REST server with SPNEGO token', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: 'FILE:/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.spnego
          service_fqdn: 'rest.krb.local'
        , (err, token) ->
          token.should.be.String()

          http.request
            hostname: 'rest.krb.local'
            port: 8080
            path: '/'
            method: 'GET'
            headers:
              Authorization: 'Negotiate ' + token
          , (res) ->
            res.statusCode.should.be.eql(200)
            done()
          .end()


###
  describe 'function with promise', ->
    
    it 'returns SPNEGO token', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: 'FILE:/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      .catch done
      .then ({ cc_patch }) ->
        krb5.spnego
          service_fqdn: 'rest.krb.local'
        .catch done
        .then (token) ->
          token.should.be.String()
          done()
###
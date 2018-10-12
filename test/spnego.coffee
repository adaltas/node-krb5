krb5 = require '../lib/'
http = require 'http'

describe 'spnego', ->

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

    it 'authenticates to REST server with SPNEGO token (hostbased service)', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: 'FILE:/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.spnego
          hostbased_service: 'HTTP@rest.krb.local'
        , (err, token) ->
          (err is undefined).should.be.true()
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

    it 'authenticates to REST server with SPNEGO token (service principal)', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: 'FILE:/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.spnego
          service_principal: 'HTTP/rest.krb.local@KRB.LOCAL'
        , (err, token) ->
          (err is undefined).should.be.true()
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

    it 'authenticates to REST server with SPNEGO token (service fqdn)', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: 'FILE:/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.spnego
          service_fqdn: 'rest.krb.local'
        , (err, token) ->
          (err is undefined).should.be.true()
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

    it 'authenticates to REST server with SPNEGO token given ccache name', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      , (err, ccname) ->
        krb5.spnego
          hostbased_service: 'HTTP@rest.krb.local'
          ccname: '/tmp/customcc'
        , (err, token) ->
          (err is undefined).should.be.true()
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

  describe 'function with promise', ->
    
    it 'returns SPNEGO token', (done) ->
      krb5.kinit
        principal: 'rest/rest.krb.local'
        keytab: '/tmp/krb5_test/rest.service.keytab'
        realm: 'KRB.LOCAL'
      .then (ccname) ->
        krb5.spnego
          hostbased_service: 'HTTP@rest.krb.local'
      .then (token) ->
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
      .catch done
      return
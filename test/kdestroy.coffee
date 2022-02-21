krb5 = require '../lib/'

ccacheStartName = process.env.DEFAULT_CCACHE_LOCATION || '/tmp'
customcc = ccacheStartName + "/customcc"
restKeytab = process.env.REST_KEYTAB || '/tmp/krb5_test/rest.service.keytab'

describe 'kdestroy', ->

  describe 'function with callback', ->

    it 'destroys default credential cache', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.kdestroy (err) ->
          return done err if err
          krb5.kdestroy (err) ->
            (err is undefined).should.be.false()
            err.message.should.startWith 'No credentials cache found'
            done()

    it 'destroys given credential cache', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
        ccname: '/tmp/customcc'
      , (err, ccname) ->
        krb5.kdestroy
          ccname: '/tmp/customcc'
        , (err) ->
          return done err if err
          krb5.kdestroy
            ccname: '/tmp/customcc'
          , (err) ->
            (err is undefined).should.be.false()
            err.message.should.startWith 'No credentials cache found'
            done()

  describe 'function with promise', ->

    it 'destroys default credential cache', ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      .then (ccname) ->
        krb5.kdestroy()
      .then ->
        krb5.kdestroy()
      .then ->
        throw Error 'Should not be able to redestroy cache'
      .catch (err) ->
        err.message.should.startWith 'No credentials cache found'

    it 'destroys given credential cache', ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
        ccname: "/tmp/customcc"
      .then (ccname) ->
        krb5.kdestroy
          ccname: "/tmp/customcc"
      .then ->
        krb5.kdestroy
          ccname: "/tmp/customcc"
      .then ->
        throw Error 'Should not be able to redestroy cache'
      .catch (err) ->
        err.message.should.startWith 'No credentials cache found'

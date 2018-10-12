krb5 = require '../lib/'


describe 'kdestroy', ->

  describe 'function with callback', ->

    it 'destroys default credential cache', (done) ->
      krb5.kinit
        principal: 'admin'
        password: 'adm1n_p4ssw0rd'
        realm: 'KRB.LOCAL'
      , (err, ccname) ->
        krb5.kdestroy (err) ->
          (err is undefined).should.be.true()
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
        ccname.should.be.eql '/tmp/customcc'
        krb5.kdestroy
          ccname: '/tmp/customcc'
        , (err) ->
          (err is undefined).should.be.true()
          krb5.kdestroy
            ccname: '/tmp/customcc'
          , (err) ->
            (err is undefined).should.be.false()
            err.message.should.startWith 'No credentials cache found'
            done()

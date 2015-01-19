#!/usr/bin/env coffee

krb = require '../index.js'

k = new krb.Krb5
  client_principal: 'pierre@HADOOP.ADALTAS.COM'
  password: 'pierre123'
  service_principal: 'HTTP@hadoop.adaltas.com'
k.kinit (err) ->
  console.log "kinit: #{err}" if err?
  k.token (err, token) ->
    console.log "token: #{err}" if err?
    console.log "token: #{token}" if token?

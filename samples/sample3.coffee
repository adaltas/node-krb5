#!/usr/bin/env coffee

krb = require '../index.js'
exec = require('child_process').exec

k = new krb.Krb5
  client_principal: 'pierre@HADOOP.ADALTAS.COM'
  password: 'pierre123'
  service_principal: 'HTTP@hadoop.adaltas.com'
console.log 'kinit...'
k.kinit (err) ->
  if err?
    console.log "kinit: #{err}\n"
  else
    console.log 'kinit ok\n'
  exec 'klist', (err, stdout, stderr) ->
    if err?
      console.log "err: #{stderr}" if err?
    else
      console.log '*** klist ***'
      console.log "#{stdout}*************"
    k.kdestroy (err) ->
      if err?
        console.log "kdestroy: #{err}\n"
      else
        console.log 'kdestroy ok\n'
      exec 'klist', (err, stdout, stderr) ->
        if err?
          console.log "err: #{stderr}" if err?
        else
          console.log '*** klist ***'
          console.log "#{stdout}*************"

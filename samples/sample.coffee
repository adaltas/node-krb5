#!/usr/bin/env coffee

http = require 'http'
krb = require '../index.js'

# Configuration
user = 'pierre@HADOOP.ADALTAS.COM'
passwd = ''
host = 'hadoop.adaltas.com' 

www_opt =
  host: host
  port: 50070
  path: '/webhdfs/v1/user/pierre/?op=LISTSTATUS'

console.log '**** INFO ****'
console.log 'This test operates a get on a host. If the response code is 401, the test acquire a kerberos credentials, wrap it in a spnego token, sends it to the host then prints the response'
console.log '**************'

http.get www_opt, (res) ->
  console.log "1. RESPONSE : #{res.statusCode}"
  if res.statusCode is 401
    krb
      user_principal: user
      user_password: passwd
      service_principal: "HTTP@#{host}"
    , (err, spnego_token) ->
      www_opt.headers =
        'Authorization': "Negotiate #{spnego_token}"
      http.get www_opt,(res) ->
        console.log '2. RESPONSE :',res.headers
        res.on 'data', (data) ->
          console.log "BODY: #{data}"
          process.exit 0

#!/usr/bin/env coffee

http = require 'http'
Kerberos = require '../index.js'

# Configuration
user='pierre'
realm='HADOOP.ADALTAS.COM'
host='hadoop.adaltas.com'
keytab=''
passwd= ''


krb = new Kerberos user,realm
#krb.getCredentialsByKeytab keytab
#krb.getCredentialsByPassword passwd

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
    krb.generateSpnegoToken host
    www_opt.headers =
      'Authorization': "Negotiate #{krb.token}"
    http.get www_opt,(res2) ->
      console.log '2. RESPONSE :',res2.headers
      res2.on 'data', (data) ->
        console.log "BODY: #{data}"
        process.exit 0

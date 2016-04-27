#!/usr/bin/env node

var url = require('url');
var util = require('util');
var krb5 = require('../lib');
var yargs = require('yargs');

/*
Example:

./samples/spnego_async.js \
  -u https://master1.ryba:50470/webhdfs/v1/?op=LISTSTATUS \
  -p hdfs@HADOOP.RYBA \
  -w hdfs123
*/

var argv = yargs
    .usage('Usage: $0 -p [principal] -w [password] -u [url]')
    .demand(['url', 'principal', 'password'])
    .alias('u', 'url')
    .describe('u', 'HTTP or HTTPS URL')
    .alias('p', 'principal')
    .describe('p', 'Kerberos Principal')
    .alias('w', 'password')
    .describe('w', 'Kerberos Password')
    .argv;

var options = url.parse(argv.url);
var http = require(options.protocol.substr(0, options.protocol.length-1));

if(options.protocol === 'https:'){
  options.rejectUnauthorized = false
}

http.get(options, function(res){
  // Make sure we are testing a SPNEGO enabled server
  if(res.statusCode !== 401){
    return process.stderr.write('The request does not accept SPNEGO.' + '\n');
  }
  // Consume the stream
  res.on('readable', function(){
    while(chunk = res.read()){}
  })
  // Get the token
  krb5.spnego({
    principal: argv.principal,
    password: argv.password,
    service_principal: 'HTTP@' + options.hostname
  }, function(err, token){
    if(err){ return process.stderr.write('ERROR: ' + err.message + '\n'); }
    options.headers = {
      'Authorization': 'Negotiate ' + token
    }
    http.get(options, function(res){
      // Read the HTTP response
      data = ''
      res.on('readable', function(){
        while(chunk = res.read()){
          data += chunk.toString()
        }
      }).on('end', function(){
        process.stdout.write(util.inspect(JSON.parse(data), {depth: null, colors: true}));
      });
    }).on('error', function(){
      process.stderr.write('ERROR: ' + err.message + '\n');
    });
  });
}).on('error', function(err){
  process.stderr.write('ERROR: ' + err.message + '\n');
});

#!/usr/bin/env node

var url = require('url');
var util = require('util');
var krb5 = require('./lib');
var yargs = require('yargs');
var i=0

var options = url.parse('https://master1.ryba:60010');

var http = require(options.protocol.substr(0, options.protocol.length-1));

if(options.protocol === 'https:'){
  options.rejectUnauthorized = false
}
options.headers = {
  'Authorization': 'Negotiate '
}

var i=0
var loop = function(){ 
  console.log("Round:",i);
  console.log('\n');
  tokenize();
  i++;
  if(i<2000) setTimeout(loop, 10);
  console.log('-----------------------------------------');
}

var tokenize = function(){
  krb5.spnego({
    principal: 'hbase@HADOOP.RYBA',
    password: 'hbase123',
    service_principal: 'HTTP@' + options.hostname
  }, function(err, token){
    if(err){ return process.stderr.write('ERROR: ' + err.message + '\n'); }
    http.get(options, function(res){
      // Read the HTTP response
      data = ''
      res.on('readable', function(){
        while(chunk = res.read()){
          data += chunk.toString()
        }
      }).on('end', function(){
        console.log("token:", token);
        //process.stdout.write(util.inspect(JSON.parse(data), {depth: null, colors: true}));
      });
    }).on('error', function(){
      process.stderr.write('ERROR: ' + err.message + '\n');
    });
  });
}

loop();

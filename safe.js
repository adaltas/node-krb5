#!/usr/bin/env node

var url = require('url');
var util = require('util');
var krb5 = require('./lib');
var yargs = require('yargs');
var i=0;

var options = url.parse('https://master1.ryba:60010/');

var http = require(options.protocol.substr(0, options.protocol.length-1));

if(options.protocol === 'https:'){
  options.rejectUnauthorized = false
}

options.headers = {
  'Authorization': 'Negotiate '
};

options.service_principal= 'HTTP@' + options.hostname
options.principal='hbase@HADOOP.RYBA';
options.password='hbase123';

var tokenize = function(){
  console.log("Round:",i);
  i++;
  k.token(function(err, token){
    if(err) return process.stderr.write('ERROR: ' + err.message + '\n');
    http.get(options, function(res){
      // Read the HTTP response
      data = ''
      res.on('readable', function(){
        while(chunk = res.read()){
          data += chunk.toString()
        }
      }).on('end', function(){
        console.log("token:", token);
        console.log('--------------------------------------------------------');
        tokenize();
        //process.stdout.write(util.inspect(JSON.parse(data), {depth: null, colors: true}));
      });
    }).on('error', function(){
      process.stderr.write('ERROR: ' + err.message + '\n');
    });
  });
};

console.log('start');
var k = krb5(options,tokenize);
console.log('inited');

#!/usr/bin/env node

var util = require('util');
var krb5 = require('../lib');
var yargs = require('yargs');
var exec = require('child_process').exec

/*
Example:

./samples/kinit_kdestroy.js \
  -p hdfs@HADOOP.RYBA \
  -w hdfs123
*/

var argv = yargs
    .usage('Usage: $0 -p [principal] -w [password] -u [url]')
    .demand(['principal', 'password'])
    .alias('p', 'principal')
    .describe('p', 'Kerberos Principal')
    .alias('w', 'password')
    .describe('w', 'Kerberos Password')
    .argv;

var k = krb5({
  principal: argv.principal,
  password: argv.password,
});

var do_kinit = function(){
  process.stderr.write('---------------------------- kinit\n');
  k.kinit(function(err){
    if(err){
      return process.stderr.write('ERROR' + err.message + '\n');
    }
    do_klist();
  });
};

var do_klist = function(){
  process.stderr.write('---------------------------- klist:\n');
  exec('klist', function(err, stdout, stderr){
    if(err){
      return process.stderr.write('ERROR: ' + err.message + '\n');
    }
    process.stderr.write(stdout + '\n');
    do_kdestroy();
  });
};

var do_kdestroy = function(){
  process.stderr.write('---------------------------- kdestroy\n');
  k.kdestroy(function(err){
    if(err){
      return process.stderr.write('ERROR: ' + err.message + '\n');
    }
    do_klist();
  });
};

do_kinit()

#!/usr/bin/env node

var util = require('util');
var krb5 = require('../lib');
var yargs = require('yargs');
var exec = require('child_process').exec;

/*
Example:

./samples/kinit_kdestroy.js \
  -p hdfs@HADOOP.RYBA \
  -w hdfs123
*/

var argv = yargs
    .usage('Usage: $0 -p [principal] -w [password] -n [fqdn]')
    .demand(['principal', 'password'])
    .alias('p', 'principal')
    .describe('p', 'Kerberos Principal')
    .alias('w', 'password')
    .describe('w', 'Kerberos Password')
    .alias('n', 'fqdn')
    .describe('n', 'FQDN of desired service host')
    .argv;

var k = krb5({
  principal: argv.principal,
  password: argv.password,
  service_fqdn: argv.fqdn
});

var do_klist = function(err){
  if(err){
    return process.stderr.write('ERROR: ' + err.message + '\n');
  }
  process.stderr.write('----------------------- klist:\n\n');
  exec('klist', function(err, stdout, stderr){
    if(err){
      return process.stderr.write('ERROR: ' + err.message + '\n');
    }
    process.stderr.write(stdout + '\n');
    do_spnego();
  });
};

var do_kinit = function(){
  process.stderr.write('----------------------- kinit:\n\n');
  k.kinit(do_klist);
};

var do_kdestroy = function(){
  process.stderr.write('-------------------- kdestroy:\n\n');
  k.kdestroy(function(err){
    if(err){
      return process.stderr.write('ERROR: ' + err.message + '\n');
    }
  });
};

var do_spnego = function(){
  if(argv.fqdn){
    process.stderr.write('---------------------- spnego:\n\n');
    k.token(function(err,token){
      if(err){
        return process.stderr.write('ERROR: ' + err.message + '\n');
      }
      process.stderr.write('TOKEN:' + token + '\n\n');
      do_kdestroy();
    });
  }
};

do_kinit();
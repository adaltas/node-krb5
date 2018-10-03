[![Build Status](https://secure.travis-ci.org/adaltas/node-krb5.svg)](http://travis-ci.org/adaltas/node-krb5)

# Kerberos for Node.js

krb5 is a Node.js native binding for Kerberos. It is a Node.js implementation of Kerberos client tools: 
* **kinit** (keytab, or password): retrieve initial credentials;
* **kdestroy**: destroy a credential cache;
* **spnego**: generate a SPNEGO token.

It uses the [MIT Kerberos] native library.  
[SPNEGO] is a GSS mechanism to authenticate through HTML requests.

## Installation

To have this module working properly, you should get MIT Kerberos. We recommand avoiding your distribution package as it may not be the MIT version, and future versions of this module may include some MIT Kerberos extensions. 

Compiling from the source of MIT Kerberos requires `python2`, `make`, `gcc` (`g++`), `bison` and `byacc`. Clone this repository and then run:
```bash
# To download, compile and install MIT Kerberos 
npm run krb5-lib 
# To compile and install the module
npm install
```

If MIT Kerberos is already installed on your system, just run `npm install krb5`.

If you want to install MIT Kerberos in another directory (default is "/usr/local"), specify a `--prefix` option to `./configure` in the file `install_krb5.sh` before running `npm run krb5-lib`. 

If kerberos is installed in a directory not included in include and/or library path (if you have manually compiled kerberos in a specific directory for example), please modify the binding.gyp present in the package root folder with the following properties:

```js
{
  'targets': [{
    'target_name': 'krb5',
    'include_dirs': [
      '/path/to/kerberos/include/dir/',
      '/path/to/kerberos_gssapi/include/dir/'],
    'libraries': [
      '/path/to/libkrb5', 
      '/path/to/libgssapi_krb5']
  }]
}
```


## Windows

To compile this library in windows, you need a complete visual studio compile chain, please refer to this [webpage][visual studio]. If you have a 32 bit OS, please delete `binding.gyp` and rename `_binding32.gyp` before install.


# Usage

## Quick example

* Retrieve a **SPNEGO token** for a service   

In this example we want to retrieve a token to access a REST API of the service HBase, located on the host `m01.krb.local` .

```js
// Get the initial credentials using a keytab
krb5.kinit({
  principal: 'hbase/m01.krb.local',
  keytab: '/tmp/hbase.service.keytab',
  realm: 'KRB.LOCAL',
}, function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
    // Get the SPNEGO token
    krb5.spnego({
      service_fqdn: 'm01.krb.local'
    }, function (err, token) {
      if (err) {
        console.log(err)
      } else {
        console.log('SPNEGO token :', token)
      }
    })
  }
});
```

**For better readability**, configure a `krb5` instance and chain methods:
```js
krb5({
  principal: 'hbase/m01.krb.local',
  keytab: '/tmp/hbase.service.keytab',
  realm: 'KRB.LOCAL',
  service_fqdn: 'm01.krb.local'
}).kinit(function (err, ccname) {
  if (err) {
    console.log(err)
  } else {
    console.log('Credentials saved in', ccname)
  }
}).spnego(function (err, token) {
  if (err) {
    console.log(err)
  } else {
    console.log('SPNEGO token :', token)
  }
});
```
Note that with this syntax, if `kinit` fails (an error code is defined), the rest of the call chain is automatically interrupted, hence `spnego` is not called. 

For more example, see the [samples][samples] and [test][test] directories.

## Functions


### `kinit(options, callback)` : retrieve initial credentials (*Ticket Granting Ticket*)


Options:  
* `principal`   
Kerberos principal *username@REALM* or *username*. If realm is given, overrides the realm option.

* `password` / `keytab`   
One of both should be given for authentication.

* `realm` (optionnal)  
Kerberos realm (usually capitalized domain name). If this is not specified, use the default realm from `/etc/krb5.conf`.

* `ccname` (optionnal)  
Credential cache location. If this is not specified, default path is taken from environment variable `KRB5CCNAME`, then from `/etc/krb5.conf`. Current implementation uses process environment variable and is **not thread safe**. Only use if you want to switch path once for all. 

Callback parameters:
* `err`  
Should be `undefined`. Otherwise it contains an error message.  

* `ccname`  
Credential path location used to store initial credentials. 

### `spnego(options, callback)` : retrieve a SPNEGO token. 

In order to retrieve a SPNEGO token to access a service, you first need an initial ticket (TGT) which you can get with `kinit`.


Options:
* `service_fqdn`   
Fully qualified domain name of the service.

* `ccname` (optionnal)  
Location of the credential cache storing the initial ticket. If not specified, default path is taken. 

Callback parameters:
* `err`  
Should be `undefined`. Otherwise contains GSS API major error code.

* `token`  
The SPNEGO token to access the service. It can then be added to the header of the HTTP request `Authorization: Negociate {token}`


### `kdestroy (options, callback)` : destroys credential cache

Options:  
* `ccname` (optionnal)  
Credential cache location. If this is not specified, default path is taken from environment variable `KRB5CCNAME`, then from `/etc/krb5.conf`. 

Callback parameters:
* `err`  
Should be `undefined`. Otherwise it contains an error message.  

___



[MIT Kerberos]: http://web.mit.edu/kerberos/
[SPNEGO]: http://en.wikipedia.org/wiki/SPNEGO
[MIT Kerberos Dist]: http://web.mit.edu/kerberos/dist/
[visual studio]:https://github.com/TooTallNate/node-gyp/wiki/Visual-Studio-2010-Setup
[samples]: https://github.com/adaltas/node-krb5/tree/master/samples
[test]: https://github.com/adaltas/node-krb5/tree/master/test

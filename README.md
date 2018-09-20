# Kerberos for Node.js

krb5 is a Node.js native binding for kerberos. It is a node.js implementation of
kerberos client tools, such as :
- kinit (keytab, or password)
- kdestroy
It uses the [MIT Kerberos] native library.
It is also able to generate a SPNEGO token. [SPNEGO] is a GSS mechanism to
authenticate through HTML requests.
Please see the first example in the samples directory for a concrete use case.

## Installation

The installation assumes that you have mit-kerberos installed on your computer. 
On Linux, you shall be all set. On Windows or Mac OS, please read the
corresponding section below. Once mit-kerberos is installed, run

```bash
npm install
```

## Windows

To compile this library in windows, you need a complete visual studio compile
chain, please refer to this [webpage][visual studio]. If you have a 32 bit OS,
please delete binding.gyp and rename \_binding32.gyp before install.

## Mac OS X

If you encounter troubles with your kerberos version, please compile kerberos
using the following instructions.

Your include path must contain:
krb5.h
*   *gssapi.h*
*   *gssapi/gssapi_krb5.h*

Your library path must contain:
*   *krb5* library
*   *gssapi_krb5* library

Here's some instructions on how to install the Kerberos and GSS libraries. You
can download the latest version of the [MIT Kerberos Distribution][MIT Kerberos Dist].
Unless you specify the "--prefix" option to `configure`, the library will be
installed inside "/usr/local".

```bash
wget http://web.mit.edu/kerberos/dist/krb5/1.15/krb5-1.15-beta1.tar.gz 
tar -xzf krb5-1.15-beta1.tar.gz 
cd krb5-1.15-beta1/src
./configure
make
sudo make install
```

If kerberos is not installed in a directory not included in include and/or library path (if you have manually
compiled kerberos in a specific directory for example), please modify the
binding.gyp present in the package root folder with the following properties:

```js
{
  'targets': [{
    'target_name': 'krb5',
    'include_dirs': ['/path/to/kerberos/include/dir/','/path/to/kerberos_gssapi/include/dir/'],
    'libraries': ['/path/to/libkrb5', '/path/to/libgssapi_krb5']
  }]
}
```

# Usage

## Quick example

* Retrieve a **SPNEGO token** for a service   

In this example we want to retrieve a token to access a REST API of the service HBase, located on the host `m01.krb.local` .

```js
// Get the initial credentials using a keytab
krb5.kinit({
  username: 'hbase/m01.krb.local',
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

**For better readability**, configure a `krb5` instance and chain methods :
```js
krb5({
  username: 'hbase/m01.krb.local',
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
Note that with this syntax, if `kinit` fails (an error code is defined), the rest of the call chain is automatically interupted, hence `spnego` is not called. 

For more example, see the [samples][samples] and [test][test] directories.

## Functions


### `kinit(options, callback)` : retrieve initial credentials (*Ticket Granting Ticket*)


Options :  
* `username`   
Kerberos principal.

* `password` / `keytab`   
One of both should be given for authentication.

* `realm`  
Kerberos realm (usually capitalize domain name).

* `ccname` (optionnal)  
Credential cache location. If this is not specified, default path is taken from environment variable `KRB5CCNAME`, then from `/etc/krb5.conf`. Current implementation uses process environment variable and is **not thread safe**. Only use if you want to switch path once for all. 

Callback parameters :
* `err`  
Should be `undefined`. Otherwise it contains an error message.  

* `ccname`  
Credential path location used to store initial credentials. 

___
### `spnego(options, callback)` : retrieve a SPNEGO token. 

In order to retrieve a SPNEGO token to access a service, you first need an initial ticket (TGT). You can get with `kinit`.


Options :
* `service_fqdn`   
Fully qualified domain name of the service.

* `ccname` (optionnal)  
Location of the credential cache storing the initial ticket. If not specified, default path is taken. 

Callback parameters :
* `err`  
Should be `undefined`. Otherwise contains GSS API major error code.

* `token`  
The SPNEGO token to access the service. It can then be added to the header of the HTTP request `Authorization: Negociate {token}`

___



[MIT Kerberos]: http://web.mit.edu/kerberos/
[SPNEGO]: http://en.wikipedia.org/wiki/SPNEGO
[MIT Kerberos Dist]: http://web.mit.edu/kerberos/dist/
[visual studio]:https://github.com/TooTallNate/node-gyp/wiki/Visual-Studio-2010-Setup
[samples]: https://github.com/adaltas/node-krb5/tree/master/samples
[test]: https://github.com/adaltas/node-krb5/tree/master/test

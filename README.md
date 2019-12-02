[![Build Status](https://secure.travis-ci.org/adaltas/node-krb5.svg)](http://travis-ci.org/adaltas/node-krb5)

# Kerberos for Node.js

krb5 is a Node.js native binding for Kerberos. It is a Node.js implementation of Kerberos client tools: 
* **kinit** (keytab, or password): retrieve initial credentials;
* **kdestroy**: destroy a credential cache;
* **spnego**: generate a SPNEGO token.

It uses the [MIT Kerberos](http://web.mit.edu/kerberos/) native library.  
[SPNEGO](http://en.wikipedia.org/wiki/SPNEGO) is a GSS mechanism to authenticate through HTML requests.

## Installation

To build this module, you need MIT Kerberos library. Refer to the section corresponding to your operating system:

* Archlinux
  ```
  pacman -S krb5
  npm install krb5
  ```
* RHEL/Centos
  ```
  yum install -y krb5-devel
  npm install krb5
  ```
* Ubuntu
  ```
  apt-get install -y libkrb5-dev
  npm install krb5
  ```
* MacOS
  ```
  brew install krb5
  npm install krb5
  ```

Python 2 must be available in your path. You can check it by running `python --version`. It should something like "Python 2.7.16". If not, you must ensure that python 2 is used, for by placing back the original path: `PATH="/usr/bin:$PATH" npm install`.

### Windows

To compile this library in windows, you need a complete visual studio compile chain, please refer to the [node-gyp instructions](https://github.com/nodejs/node-gyp#on-windows). If you have a 32 bit OS, please delete `binding.gyp` and rename `_binding32.gyp` before install.

### z/OS

Install [curl for z/OS](https://www.rocketsoftware.com/product-categories/mainframe/curl-for-zos) and [gzip for z/OS](https://www.rocketsoftware.com/product-categories/mainframe/gzip-zos), then follow the instruction [here](#install-node-krb5-on-zos).

### Manual compilation of MIT Kerberos

Follow these instructions if you wish to manually install MIT Kerberos (in case your distribution packet manager does not have a corresponding package for example).

```
wget https://kerberos.org/dist/krb5/1.16/krb5-1.16.1.tar.gz
tar -xzf krb5-1.16.1.tar.gz 
cd krb5-1.16.1/src
./configure
make
sudo make install
```
The latest version downloaded with `wget` can be found [here](https://web.mit.edu/kerberos/).


Compiling from the source of MIT Kerberos requires `python2`, `make`, `gcc` (`g++`), `bison`.

If you want to install MIT Kerberos in another directory (default is "/usr/local"), specify a `--prefix` option to `./configure`.

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

### Install node-krb5 on z/OS

1. run the following command to install krb5 libraries for z/OS (it requires [curl for z/OS](https://www.rocketsoftware.com/product-categories/mainframe/curl-for-zos) and [gzip for z/OS](https://www.rocketsoftware.com/product-categories/mainframe/gzip-zos))
```
_ENCODE_FILE_NEW=BINARY curl https://codeload.github.com/ibmruntimes/libkrb5-zos/tar.gz/v1.16.3-zos --output v1.16.3-zos.tar.gz
gzip -d v1.16.3-zos.tar.gz
tar -xfUXo v1.16.3-zos.tar
chtag -tc 819 -R ./libkrb5-zos-1.16.3-zos
chtag -b -R ./libkrb5-zos-1.16.3-zos/lib
```

2. set the following environment variable

```
export KRB5_HOME=/path/to/libkrb5-zos-1.16.3-zos
```

3. `npm install node-krb5`

Remember to specify your `krb5.conf`:

```
export KRB5_CONFIG=/path/to/krb5.conf
```

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

You can also use promises.

```js
krb5.kinit({
  principal: 'hbase/m01.krb.local',
  keytab: '/tmp/hbase.service.keytab',
  realm: 'KRB.LOCAL',
}).then(function (ccname) {
  console.log('Credentials saved in', ccname)
  return krb5.spnego({
    hostbased_service: 'HTTP@m01.krb.local'
  })
}).then(function (token) {
  console.log('SPNEGO token :', token)
}).catch(function (err) {
  console.log(err)
})
```

For more examples, see the [samples](https://github.com/adaltas/node-krb5/tree/master/samples) and [test](https://github.com/adaltas/node-krb5/tree/master/test) directories.

## Functions

### `kinit(options, callback)`

Retrieve initial credentials (*Ticket Granting Ticket*)

Options:

* `principal`   
  Kerberos principal *username@REALM* or *username*. If realm is given, overrides the realm option.
* `password` / `keytab`   
  One of both should be given for authentication.
* `realm` (optionnal)   
  Kerberos realm (usually capitalized domain name). If this is not specified, use the default realm from `/etc/krb5.conf`.
* `ccname` (optionnal)   
  Credential cache location. If this is not specified, default path is taken from environment variable `KRB5CCNAME`, then from `/etc/krb5.conf`. 

Callback parameters:

* `err`   
  Should be `undefined`. Otherwise it contains an error message.  
* `ccname`   
  Credential path location used to store initial credentials. 

### `spnego(options, callback)`

Retrieve a SPNEGO token. 

In order to retrieve a SPNEGO token to access a service, you first need an initial ticket (TGT) which you can get with `kinit`.

Options:

* `hostbased_service` or `service_fqdn`   
  Hostbased service should be of the form `service@fqdn`. If you only pass the fully qualified domain name `fqdn`, it will default to `HTTP@fqdn`.   
  It will be resolved to the corresponding principal `service/fqdn@REALM` by the GSS-API. To use the principal directly, use the `service_principal` option instead.
* `service_principal`   
  Principal of the service.   
* `ccname` (optionnal)  
  Location of the credential cache storing the initial ticket. If not specified, default path is taken. 

Callback parameters:

* `err`   
  Should be `undefined`. Otherwise contains GSS API major error code.
* `token`   
  The SPNEGO token to access the service. It can then be added to the header of the HTTP request `Authorization: Negociate {token}`.

### `kdestroy (options, callback)` : destroys credential cache

Options:

* `ccname` (optionnal)   
  Credential cache location. If this is not specified, default path is taken from environment variable `KRB5CCNAME`, then from `/etc/krb5.conf`. 

Callback parameters:

* `err`   
  Should be `undefined`. Otherwise it contains an error message.  


## Run the tests

To run the tests in a container:

```bash
cd docker && ./run_tests $os
```

Available `$os`: archlinux / ubuntu / centos7

To test this module locally, run the KDC and REST dockers, and use the corresponding krb5.conf (bcakup your own if you need it later):

```bash
cd docker
docker-compose up -d kerberos
docker-compose up -d rest
sudo mv /etc/krb5.conf /etc/krb5.conf.backup
sudo cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
sudo npm test
```

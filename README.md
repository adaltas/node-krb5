krb5 is a Node.js native binding for kerberos. It is a node.js implementation of kerberos client tools, such as :
- kinit (keytab, or password)
- kdestroy
It uses the [MIT Kerberos] native library.
It is also able to generate a SPNEGO token. [SPNEGO] is a GSS mechanism to authenticate through HTML requests.
Please see the first example in the samples directory for a concrete use case.

# Installation

The installation assumes that you have mit-kerberos installed on your computer. If you are on Linux, just do
```bash
npm install
```
If you are on Windows or Mac OS, please read the corresponding section

## Windows

To compile this library in windows, you need a complete visual studio compile chain, please refer to this [webpage][visual studio].
If you have a 32 bit OS, please delete binding.gyp and rename _binding32.gyp before install.

## Mac OS X

If you encounter troubles with your kerberos version, please compile kerberos using the following instructions.

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
wget http://web.mit.edu/kerberos/dist/krb5/1.13/krb5-1.13-signed.tar
tar -xf krb5-1.13-signed.tar
tar -xzf krb5-1.13.tar.gz
cd krb5-1.13/src
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

## Simple Style

```coffeescript
krb5 = require 'krb5'

krb5
  client_principal
  password
  keytab  # ignored if password is set. If not and user_keytab not set, default keytab is used
  service_principal
, (err, spnego_token) ->
  console.log spnego_token
```
## Advanced Style

```coffeescript
ks = new krb5.Krb5
  client_principal
  password
  keytab  # ignored if password is set. If not and user_keytab not set, default keytab is used
  service_principal
  renew: true||false
ks.kinit options, (err) ->
ks.kdestroy (err) ->
ks.token (err, token) ->
  console.log token
```

for more example, see the samples directory
[MIT Kerberos]: http://web.mit.edu/kerberos/
[SPNEGO]: http://en.wikipedia.org/wiki/SPNEGO
[MIT Kerberos Dist]: http://web.mit.edu/kerberos/dist/
[visual studio]:https://github.com/TooTallNate/node-gyp/wiki/Visual-Studio-2010-Setup
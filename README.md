krb5 is a Node.js native binding for kerberos. It is a node.js implementation of kerberos client tools, such as :
- kinit (keytab, or password)
- kdestroy
It uses the [MIT Kerberos] native library.
It is also able to generate a SPNEGO token. [SPNEGO] is a GSS mechanism to authenticate through HTML requests.
Please see the first example in the samples directory for a concrete use case.

# Installation

IF YOU USE MAC OS X, PLEASE MANUALLY COMPILE MIT KERBEROS, AND MODIFY BINDING.GYP AS EXPLAINED BELOW.

If you encounter troubles with your kerberos version, please compile kerberos using [MIT Kerberos sources].

The installation assumes that you have mit-kerberos installed on your computer.
Your default include directories must contain:
krb5.h
- *gssapi.h*
- *gssapi/gssapi_krb5.h*

Your default library directories must contain:
- *krb5* library
- *gssapi_krb5* library

If kerberos is not installed in one of theses directories (if you have manually compiled kerberos for example), please modify in binding.gyp :

```js
'include_dirs': ['/path/to/kerberos/include/dir/','/path/to/kerberos_gssapi/include/dir/'],

'libraries': ['/path/to/libkrb5', '/path/to/libgssapi_krb5']
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
[MIT Kerberos sources]: http://web.mit.edu/kerberos/dist/krb5/1.13/krb5-1.13-signed.tar
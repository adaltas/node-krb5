Node.js native binding for kerberos authentification with spnego wrapping


## Usage

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
ks.klist (err, info) ->
  console.log info.valid_starting
  console.log info.expires
ks.token (err, token) ->
  console.log token
```

TODO: kdestroy, klist

## Example

```js
var Krb =require('krb_spnego');

var k = new Krb('username','realm');

k.getCredentialsByKeytabSync('keytabname'); //by keytab
k.getCredentialsByPasswordSync('password'); //by password

k.generateSpnegoToken('hostname'); //principal will be HTTP@hostname
k.generateSpnegoToken(); //host = realm

console.log(k.token);
```

Node.js native binding for kerberos authentification with spnego wrapping


## Usage

## Simple Style

```coffeescript
krb5 = require 'krb5'

krb5
  user_principal
  user_password
  user_keytab  # ignored if password is set. If not and user_keytab not set, default keytab is used
  service_principal
  renew: true||false
, (err, spnego_token) ->
  console.log spnego_token
```
## Advanced Style

```coffeescript
ks = new krb5.Krb5
  user_principal
  user_password
  user_keytab  # ignored if password is set. If not and user_keytab not set, default keytab is used
  service_principal
  renew: true||false
ks.kinit options, (err, info) ->
ks.kdestroy (err, info) ->
ks.klist (err, info) ->
  console.log info.valid_starting
  console.log info.expires
ks.spnego (err, token) ->
  console.log token
```

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


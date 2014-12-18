Node.js native binding for kerberos authentification with spnego wrapping

# Example

```
var Krb =require('krb_spnego');

var k = new Krb('username','realm');

k.getCredentialsByKeytabSync('keytabname'); //by keytab
k.getCredentialsByPasswordSync('password'); //by password

k.generateSpnegoToken('hostname'); //principal will be HTTP@hostname
k.generateSpnegoToken(); //host = realm

console.log(k.token);
```

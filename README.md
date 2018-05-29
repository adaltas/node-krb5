# node-krb5
Krb5 nodejs bindings and lib 

Debug build : 
```
npm run-script rebuild; npm run-script coffee
```

Tests :
```
mocha test/*.coffee
```

TODO : 
[ ] Mocha tests in docker
[ ] Fix Segfault/Permission denied for `default_ccache_name = KEYRING:persistent:uidnumber`
[ ] GSS-API implentation
[ ] Add Spnego support 

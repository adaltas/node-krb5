#!/bin/bash

cd /node-krb5
cat /tmp/krb5_test/krb5.conf
cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
cp /tmp/krb5_test/hbase.service.keytab 
# --unsafe-perm alloww node-gyp build as root
npm run krb5-lib
npm install --unsafe-perm
npm test
tail -f /dev/null
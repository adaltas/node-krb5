#!/bin/bash

cd /node-krb5
cat /tmp/krb5_test/krb5.conf
cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
npm run krb5-lib
# --unsafe-perm alloww node-gyp build as root
npm install --unsafe-perm
npm test
err=$?
exit $err
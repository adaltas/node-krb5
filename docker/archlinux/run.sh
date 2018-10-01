#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

cd /node-krb5
cat /tmp/krb5_test/krb5.conf
cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
npm run krb5-lib
for node_version in "10.11.0" "8.12.0" "6.14.4" "4.9.1"; do
  echo "Node.js version "$node_version
  n $node_version
  # --unsafe-perm allows node-gyp build as root
  # --force forces reinstallation
  npm install --unsafe-perm --force
  npm test
  err=$?
  if [ $err -ne 0 ] 
  then
    echo -e $RED"Tests failed for Node.js "$node_version$NC
    exit $err
  else
    echo -e $GREEN"Tests passed for Node.js "$node_version$NC
  fi
done
echo -e $GREEN"Tests passed for Node.js version 4, 6, 8 and 10"$NC
exit 0

#!/bin/bash

[[ "TRACE" ]] 

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

source scl_source enable devtoolset-10 # load environment configuration to use gcc 7

while [ ! -f /tmp/krb5_test/rest_ready ] 
do
  echo "Waiting for REST server to be ready..."
  sleep 1
done
cd /node-krb5
cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
for node_version in "16" "14"; do
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
echo -e $GREEN"Tests passed for Node.js version 14 and 16"$NC
exit 0
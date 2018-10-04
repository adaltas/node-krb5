#!/bin/bash

[[ "TRACE" ]] 

while [ ! -f /tmp/krb5_test/http.service.keytab ] 
do
  echo "Waiting for KDC to be ready..."
  sleep 3
done
cp /tmp/krb5_test/krb5.conf /etc/krb5.conf
cp /tmp/krb5_test/http.service.keytab /etc/krb5.keytab
touch /var/log/rest.log
python3.6 -u /node-krb5/src/server.py &> /var/log/rest.log &
touch /tmp/krb5_test/rest_ready
tail -F /var/log/rest.log

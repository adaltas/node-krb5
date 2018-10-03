#!/bin/bash

[[ "TRACE" ]] && set -x

: ${REALM:=KRB.LOCAL}
: ${DOMAIN_REALM:=krb.local}
: ${KERB_MASTER_KEY:=masterkey}
: ${KERB_ADMIN_USER:=admin}
: ${KERB_ADMIN_PASS:=adm1n_p4ssw0rd}
: ${SEARCH_DOMAINS:=krb.local}

fix_nameserver() {
  cat>/etc/resolv.conf<<EOF
nameserver $NAMESERVER_IP
search $SEARCH_DOMAINS
EOF
}

fix_hostname() {
  hostname kdc.$DOMAIN_REALM
}

create_config() {
  : ${KDC_ADDRESS:=$(hostname -f)}

  cat>/etc/krb5.conf<<EOF
[logging]
 default = FILE:/var/log/kerberos/krb5libs.log
 kdc = FILE:/var/log/kerberos/krb5kdc.log
 admin_server = FILE:/var/log/kerberos/kadmind.log
[libdefaults]
 default_realm = $REALM
 dns_lookup_realm = false
 dns_lookup_kdc = false
 ticket_lifetime = 24h
 renew_lifetime = 7d
 forwardable = true
 rdns = false
[realms]
 $REALM = {
  kdc = $KDC_ADDRESS
  admin_server = $KDC_ADDRESS
 }
[domain_realm]
 .$DOMAIN_REALM = $REALM
 $DOMAIN_REALM = $REALM
EOF
}

create_db() {
  /usr/sbin/kdb5_util -P $KERB_MASTER_KEY -r $REALM create -s
}

start_kdc() {
  mkdir -p /var/log/kerberos

  /usr/sbin/krb5kdc -P /var/run/krb5kdc/krb5kdc.pid
  /usr/sbin/kadmind -P /var/run/krb5kdc/kadmind.pid
}

restart_kdc() {
  kill $(cat /var/run/krb5kdc/krb5kdc.pid)
  /usr/sbin/krb5kdc start -P /var/run/krb5kdc/krb5kdc.pid
  kill $(cat /var/run/krb5kdc/kadmind.pid)
  /usr/sbin/kadmind -P /var/run/krb5kdc/kadmind.pid
}

create_admin_user() {
  kadmin.local -q "addprinc -pw $KERB_ADMIN_PASS $KERB_ADMIN_USER"
  echo "admin@$REALM *" > /var/kerberos/krb5kdc/kadm5.acl
}

create_rest_user() {
  kadmin.local -q "addprinc -randkey rest/rest.krb.local@KRB.LOCAL"
  rm -f /tmp/krb5_test/rest.service.keytab
  kadmin.local -q "xst -k /tmp/krb5_test/rest.service.keytab rest/rest.krb.local@KRB.LOCAL"

  kadmin.local -q "addprinc -randkey HTTP/rest.krb.local@KRB.LOCAL"
  rm -f /tmp/krb5_test/htpp.service.keytab
  kadmin.local -q "xst -k /tmp/krb5_test/http.service.keytab HTTP/rest.krb.local@KRB.LOCAL"
}


main() {
  fix_nameserver
  fix_hostname

  create_config
  cp /etc/krb5.conf /tmp/krb5_test/
  create_db
  create_admin_user
  create_rest_user
  start_kdc

  if [ ! -f /var/kerberos/krb5kdc/principal ]; then
    while true; do sleep 1000; done
  else
    start_kdc
    tail -F /var/log/kerberos/krb5kdc.log
  fi
}

[[ "$0" == "$BASH_SOURCE" ]] && main "$@"
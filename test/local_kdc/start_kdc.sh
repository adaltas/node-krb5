#!/usr/bin/env bash


if [[ ! -z $1 ]]; then
    basedir="$1"
else
    basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
fi


source "$basedir/conf.sh"

kill_pid "$PID_DIR/krb5kdc.pid"
kill_pid "$PID_DIR/kadmind.pid"
kill_pid "$PID_DIR/server.pid"

re_mkdir "$CONF_DIR"
re_mkdir "$LOG_DIR"
re_mkdir "$DEFAULT_CCACHE_LOCATION"
re_mkdir "$DB_LOCATION"
re_mkdir "$PID_DIR"
re_mkdir "$KEYTAB_DIR"

cat>"$KRB5_CONFIG"<<EOF
[logging]
 default = FILE:$DEFAULT_LOG_FILE
 kdc = FILE:$KDC_LOG_FILE
 admin_server = FILE:$ADMIN_SERVER_LOG_FILE
[libdefaults]
 default_ccache_name = FILE:$DEFAULT_CCACHE_LOCATION/krb5cc_%{uid}
 default_realm = KRB.LOCAL
 dns_lookup_realm = false
 dns_lookup_kdc = false
 ticket_lifetime = 24h
 renew_lifetime = 7d
 forwardable = true
 rdns = false
[dbmodules]
 KRB.LOCAL = {
  database_name = $DB_NAME
 }
[realms]
 KRB.LOCAL = {
  key_stash_file = $STASH_FILE
  acl_file = $ACL_FILE
  kdc_listen = 8088
  kdc_tcp_listen = ""
  kadmind_listen = 8749
  kpasswd_port = 8750
  kdc = localhost:8088
  admin_server = localhost:8749
 }
[domain_realm]
 .krb.local = KRB.LOCAL
 krb.local = KRB.LOCAL
EOF

echo "admin@KRB.LOCAL *" > "$ACL_FILE"


if [[ -f "$DB_NAME" ]]; then
    rm -f "$DB_NAME*"
fi

kdb5_util -d "$DB_NAME" -P masterkey create -s
sleep 2
kadmin.local -q "addprinc -pw adm1n_p4ssw0rd admin"

rm_kt "$REST_KEYTAB"
rm_kt "$HTTP_KEYTAB"


kadmin.local -q "addprinc -randkey rest/rest.krb.local@KRB.LOCAL"
kadmin.local -q "xst -k $REST_KEYTAB rest/rest.krb.local@KRB.LOCAL"
 
kadmin.local -q "addprinc -randkey HTTP/rest.krb.local@KRB.LOCAL"
kadmin.local -q "xst -k $HTTP_KEYTAB HTTP/rest.krb.local@KRB.LOCAL"


krb5kdc -P "$PID_DIR/krb5kdc.pid"
kadmind -P "$PID_DIR/kadmind.pid"
sleep 2
PYTHONUNBUFFERED=1 python3 "$basedir/../../src/server.py" -local > $LOG_DIR/server.log & echo $! > "$PID_DIR/server.pid"

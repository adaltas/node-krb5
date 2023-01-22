#!/usr/bin/env bash

function re_mkdir() {
    if [[ -d $1 ]]; then
        rm -Rf "$1"
    fi
    mkdir -p "$1"
}

function kill_pid() {
    if [[ -f "$1" ]]; then
        kill "$(cat $1)"
        rm "$1"
    fi
}

function rm_kt {
    if [[ -f "$1" ]]; then
        rm "$1"
    fi
}

if [[ ! -z $1 ]]; then
    basedir="$1"
else
    basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
fi

export CONF_DIR="$basedir/conf"
export KRB5_CONFIG="$CONF_DIR/krb5.conf"
export LOG_DIR="$basedir/logs"
export DEFAULT_LOG_FILE="$LOG_DIR/krb5libs.log"
export KDC_LOG_FILE="$LOG_DIR/krb5kdc.log"
export ADMIN_SERVER_LOG_FILE="$LOG_DIR/kadmind.log"
export DEFAULT_CCACHE_LOCATION="$basedir/ccaches"
export DB_LOCATION="$basedir/db"
export STASH_FILE="$DB_LOCATION/.k5.KRB.LOCAL"
export DB_NAME="$DB_LOCATION/principal"
export PID_DIR="$basedir/pid"
export ACL_FILE="$basedir/conf/kadm5.acl"
export KEYTAB_DIR="$basedir/keytab"
export REST_KEYTAB="$KEYTAB_DIR/rest.service.keytab"
export HTTP_KEYTAB="$KEYTAB_DIR/http.service.keytab"
export KRB5_KTNAME="FILE:$HTTP_KEYTAB" # keytab used by server.py
export REST_ADDRESS="$(hostname -f)"


if [[ 0 -ne $(command -v krb5kdc >/dev/null; echo $?) ]]; then
    if [[ ! -z "$KRB5_HOME" ]]; then
        export PATH="$KRB5_HOME/sbin:$PATH"
    fi
    if [[ 0 -ne $(command -v krb5kdc >/dev/null; echo $?) ]]; then
        echo "Fail to find krb5kdc in PATH, either you installed krb5 via homebrew then you must set KRB5_HOME to your cellar/krb5 path"
        echo "(example /opt/homebrew/opt/krb5/) or you didn't install the relevant package (krb5-kdc on debian-like)"
        echo "In case you compiled mit krb5 to a custom prefix, set the prefix as KRB5_HOME"
        exit 1
    fi
fi

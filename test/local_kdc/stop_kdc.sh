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

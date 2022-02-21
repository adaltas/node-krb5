#!/bin/bash

current_script_basedir="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
source "$current_script_basedir/local_kdc/conf.sh" "$current_script_basedir/local_kdc"
. "$current_script_basedir/local_kdc/start_kdc.sh" "$current_script_basedir/local_kdc"

npm run test

. "$current_script_basedir/local_kdc/stop_kdc.sh" "$current_script_basedir/local_kdc"

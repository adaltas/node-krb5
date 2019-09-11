#!/bin/bash
if [ `uname -s` == "OS/390" ] && [ -z "$MANUAL" ]; then
    . ./install_krb5_zos.sh
fi
node-gyp rebuild

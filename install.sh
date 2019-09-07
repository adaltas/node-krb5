#!/bin/bash
if [ "$OSTYPE" == "mvs" ]; then
    . ./install_krb5_zos.sh
fi
node-gyp rebuild

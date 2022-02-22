#!/usr/bin/env bash
set -e

vendor="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )/vendor"
if [[ ! -d "$vendor/lib" || ! -d "$vendor/include" || ! -d "$vendor/sbin" ]]; then
    echo "Downloading MIT Kerberos..."
    wget https://kerberos.org/dist/krb5/1.16/krb5-1.16.1.tar.gz
    echo "Extracting..."
    mkdir deps
    if [[ -d "$vendor" ]]; then
        rm -rf "$vendor"
    fi
    mkdir "$vendor"
    tar -xzf krb5-1.16.1.tar.gz --directory deps
    cd deps/krb5-1.16.1/src
    echo "Compiling"
    ./configure --prefix "$vendor"
    make
    echo "Installing binaries to $vendor"
    make install
    echo "Cleaning files..."
    cd ../../..
    rm -rf deps
    rm -rf krb5-1.16.1.tar.gz
else
    echo "$vendor seems to already exists, if you need to rebuild it, delete the folder"
fi

echo "Export the following variable to make the vendored krb5 available to the build"
echo "export KRB5_HOME=$vendor"

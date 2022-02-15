
# Changelog

## Version 0.5.5

* build: windows now back working
* doc: update documentation
* test: update Archlinux dockerfile with new base image
* test: remove older node versions, new versions are 14 and 16
* test: update Centos dockerfile with working dev toolchain
* test: update Rest dockerfile with new repositories
* test: update all "client" dockerfiles python to python3

## Version 0.5.4

* package: update adaltas url

## Version 0.5.3

* package: use latest versions
* build: osx now back working

## Version 0.5.2

* src: support node-krb5 on z/OS
* package: remove lock files from git
* test: improve error message in docker run command

## Version 0.5.1

* test: update Archlinux dockerfile with new base image
* src: fix init_context error handling

## Version 0.5.0

* api: support promise API
* src: remove krb5 instance
* src: add flexibility to service for SPNEGO
* test: only build required image for tests

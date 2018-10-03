#!/bin/bash -eu

set +e
if [ "$#" -eq 0 ];then
	echo "No environment set"
  exit 1
fi

docker-compose build
docker-compose up -d kerberos
docker-compose up -d rest
docker-compose run --rm $1 "/run.sh"
exitcode=$?
docker-compose down
exit $exitcode
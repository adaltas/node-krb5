#!/bin/bash -eu

if [ "$#" -eq 0 ];then
	echo "No environment set"
  exit 1
fi

docker-compose build
docker-compose run --rm $1 "/run.sh"
exitcode=$?
docker-compose down
echo $exitcode
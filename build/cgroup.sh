#!/bin/bash
set -e

if [ -z $1 ]
then
	echo "First argument should be the memory limit in bytes for cgroups to use."
	exit
fi
if [ -z $2 ]
then
	echo "Second argument should be the name of the cgroups"
	exit
fi
echo 200000000 > $"/var/cgroups/$2/memory.limit_in_bytes"
cgexec -g memory:cache-test ./cache $1 $2

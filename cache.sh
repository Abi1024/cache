#!/bin/bash
set -ex

if [ $# -ne 3 ]
then
	echo "Must supply 3 arguments. \n \
	Usage: sudo ./cache.sh <program> <cgroup_memory_in_mebibytes> <cgroup_name> \n \
	0 = test program \n \
	1 = cache_adaptive matrix multiply \n \
	2 = non_cache_adaptive matrix multiply \n"
	exit
fi

cmake ./build && make ./build
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"

if [ -d  "/var/cgroups/$3"]
	cgdelete memory:$3

cgcreate -g "memory:$3" -t abiyaz:abiyaz

case "$1" in

0)	cgexec -g memory:$1 ./test $2 $3
		;;

1)	cgexec -g memory:$1 ./cache_adaptive $2 $3
		;;

2)	cgexec -g memory:$1 ./cache_non_adaptive $2 $3
		;;

esac

echo "Success";

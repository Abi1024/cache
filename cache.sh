#!/bin/bash
set -ex

if [ $# -ne 3 ]
then
	echo "Must supply 3 arguments. \n \
	Usage: sudo ./cache.sh <program> <cgroup_memory_in_mebibytes> <cgroup_name> \n \
	The possible values for <program> are: \n \
	0 = test program \n \
	1 = cache_adaptive matrix multiply \n \
	2 = non_cache_adaptive matrix multiply \n"
	exit
fi

if [ ! -d  "./stxxl/lib" ]
then
	git clone git@github.com:stxxl/stxxl.git
fi

cmake ./build && make ./build
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"

if [ -d  "/var/cgroups/$3" ]
then
	cgdelete memory:$3
fi

cgcreate -g "memory:$3" -t abiyaz:abiyaz
#sudo bash -c "echo 10 > /var/cgroups/$3/memory.swappiness"

case "$1" in

0)	cgexec -g memory:$3 ./build/test $2 $3
		echo "Done"
		#valgrind --leak-check=full ./build/test $2 $3
		;;

1)	cgexec -g memory:$3 ./build/cache_adaptive $2 $3
		echo "Done"
		;;

2)	cgexec -g memory:$3 ./build/non_cache_adaptive $2 $3
		echo "Done"
		;;
esac

echo "Success"

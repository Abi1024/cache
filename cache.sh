#!/bin/bash
set -ex

if [ $# -ne 4 ]
then
	echo "Must supply 4 arguments. \n \
	Usage: sudo ./cache.sh <program> <length of matrix> <cgroup_memory_in_megabytes> <cgroup_name> \n \
	The possible values for <program> are: \n \
	0 = test program \n \
	1 = cache_adaptive matrix multiply \n \
	2 = non_cache_adaptive matrix multiply \n"
	exit
fi

#if [ ! -d  "./stxxl/lib" ]
#then
#	git clone git@github.com:stxxl/stxxl.git
#fi

cmake ./build && make --directory=./build
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"

if [ -d  "/var/cgroups/$4" ]
then
	cgdelete memory:$4
fi

cgcreate -g "memory:$4" -t abiyaz:abiyaz
sudo bash -c "echo 1 > /var/cgroups/$4/memory.oom_control"
#sudo bash -c "echo 10 > /var/cgroups/$3/memory.swappiness"

case "$1" in

0)	cgexec -g memory:$3 ./build/test $2 $3
		echo "Done"
		#valgrind --leak-check=full ./build/test $2 $3
		;;

1)  ./build/mm_data $2
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		cgexec -g memory:$4 ./build/cache_adaptive $2 $3 $4
		echo "Done"
		;;

2)	./build/mm_data $2
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		cgexec -g memory:$4 ./build/non_cache_adaptive $2 $3 $4
		echo "Done"
		;;
3)	cgexec -g memory:$3 ./build/cgroup_test $2 $3
		echo "Done"
		;;
4) cgexec -g memory:$3 ./build/funnel_sort $2 $3
		echo "Done"
		;;
5) cgexec -g memory:$3 ./build/merge_sort $2 $3
		echo "Done"
		;;
esac

echo "Success"

#!/bin/bash
set -ex

if [ $# -ne 1 ]
then
	echo "Must supply 1 argument. \n \
	Usage: sudo ./mm.sh <cgroup_name>"
  exit 1
fi

if [ ! -f "balloon_data" ]
then
  echo "First creating file for storing the balloon's data."
  dd if=/dev/urandom of=balloon_data count=16384 bs=1048576
fi

if [ ! -f "nullbytes" ]
then
  echo "First creating file for storing data."
  dd if=/dev/urandom of=nullbytes count=16384 bs=1048576
fi

cmake ./build && make --directory=./build

./cgroups.sh $1
echo 110100480 > /var/cgroups/$1/memory.limit_in_bytes
./build/mm_data 2000
sudo sh -c "sync"
sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
sudo sh -c "echo 0 > /proc/sys/vm/vfs_cache_pressure"
cgexec -g memory:$1 ./build/balloon > balloon_log.txt &
cgexec -g memory:$1 ./build/balloon > balloon_log2.txt &
cgexec -g memory:$1 ./build/balloon > balloon_log3.txt &
cgexec -g memory:$1 ./build/balloon > balloon_log4.txt &
cgexec -g memory:$1 ./build/balloon > balloon_log5.txt &
cgexec -g memory:$1 ./build/cache_adaptive_balloon 0 32000 5 $1

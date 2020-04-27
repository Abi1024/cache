#!/bin/bash
set -ex

NUMRUNS=3
NUMBALLOONS=9
SLEEP=5
TOTALMEMORY_MB=100
TOTALMEMORY=$((TOTALMEMORY_MB*1024*1024))
MATRIXWIDTH=4096
STARTINGMEMORY=10


if [ $# -ne 1 ]
then
	echo "Must supply 1 argument. \n \
	Usage: sudo ./mm.sh <cgroup_name>"
fi

if [ -f "mm_out.txt" ]
then
  echo "mm_out.txt already exists. Deleting it first."
  rm mm_out.txt
fi

if [ -f "balloon_log.txt" ]
then
  echo "balloon_log.txt already exists. Deleting it first."
  rm balloon_log.txt
fi


if [ -f "err" ]
then
  echo "err already exists. Deleting it first."
  rm err
fi

if [ ! -f "nullbytes" ]
then
  echo "First creating file for storing data."
  dd if=/dev/urandom of=nullbytes count=16384 bs=1048576
fi

cmake ./build && make --directory=./build

#run cache-adaptive on constant memory
./cgroups.sh $1
./build/mm_data $MATRIXWIDTH
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
for j in `seq 1 $NUMBALLOONS`;
do
	cgexec -g memory:$1 ./build/balloon2 > "balloon_log.txt" 0 $TOTALMEMORY_MB $STARTINGMEMORY 9 $j &
	#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
done
cgexec -g memory:$1 ./build/cache_adaptive_balloon 0 $MATRIXWIDTH 100 $1
pkill -f balloon2

#run non-cache-adaptive on constant memory
./cgroups.sh $1
./build/mm_data $MATRIXWIDTH
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
for j in `seq 1 $NUMBALLOONS`;
do
	cgexec -g memory:$1 ./build/balloon2 > "balloon_log.txt" 0 $TOTALMEMORY_MB $STARTINGMEMORY 9 $j &
	#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
done
cgexec -g memory:$1 ./build/non_cache_adaptive_balloon 0 $MATRIXWIDTH 100 $1
pkill -f balloon2

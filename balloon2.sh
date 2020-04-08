#!/bin/bash
set -ex

NUMRUNS=3
NUMBALLOONS=5
SLEEP=5
TOTALMEMORY=102000000
TOTALMEMORY_MB=$((TOTALMEMORY/(1024*1024) ))

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
./cgroups.sh $1
./build/mm_data 8000
sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
cgexec -g memory:$1 ./build/balloon2 > "balloon_log.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log2.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log3.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log4.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log5.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log6.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log7.txt" &
cgexec -g memory:$1 ./build/balloon2 > "balloon_log8.txt" &
cgexec -g memory:$1 ./build/cache_adaptive_balloon 0 16000 100 $1
#pkill -f balloon
#cgexec -g memory:$1 ./build/balloon2

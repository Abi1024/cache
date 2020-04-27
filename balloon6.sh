#!/bin/bash
set -ex

NUMRUNS=3
NUMBALLOONS=9
SLEEP=5
declare -a matrixwidth=( 4096   )
declare -a startingmemory=( 15 )
TOTALMEMORY_MB=100
TOTALMEMORY=$((TOTALMEMORY_MB*1024*1024))

if [ $# -ne 1 ]
then
	echo "Must supply 1 argument. \n \
	Usage: sudo ./mm.sh <cgroup_name>"
fi

if [ -f "effect.txt" ]
then
  echo "effect.txt already exists. Deleting it first."
  rm effect.txt
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

for (( index=0; index<=${#matrixwidth[@]}-1; index++ ));
do
	echo $index
  MATRIXWIDTH=${matrixwidth[$index]}
  STARTINGMEMORY_MB=${startingmemory[$index]}

	echo "Running non cache adaptive to generate memory profiles"
	./cgroups.sh $1
	./build/mm_data $MATRIXWIDTH
	sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
	echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
	for j in `seq 1 $NUMBALLOONS`;
	do
		cgexec -g memory:$1 ./build/balloon2 > "balloon_log.txt" 2 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
		#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
	done
	cgexec -g memory:$1 ./build/non_cache_adaptive_balloon 0 $MATRIXWIDTH $STARTINGMEMORY_MB $1
	pkill -f balloon2


done

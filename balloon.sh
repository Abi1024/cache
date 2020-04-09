#!/bin/bash
set -ex

NUMRUNS=3
NUMBALLOONS=5
SLEEP=5
TOTALMEMORY=102000000
TOTALMEMORY_MB=$((TOTALMEMORY/(1024*1024) ))
declare -a matrixwidth=( 4000)
declare -a startingmemory=( 10 )

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

for (( index=0; index<=${#matrixwidth[@]}-1; index++ ));
do
  echo $index
  MATRIXWIDTH=${matrixwidth[$index]}
  STARTINGMEMORY=${startingmemory[$index]}
  for i in `seq 1 $NUMRUNS`;
  do
    #run non-cache-adaptive on constant memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			if [ ! -f "balloon_data$j" ]
			then
			  echo "First creating file for storing the balloon's data."
			  dd if=/dev/urandom of="balloon_data$j" count=1000 bs=1048576
			fi

		done

		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon > "balloon_log$j.txt" 0 100 30 $NUMBALLOONS $j  &
		done
    cgexec -g memory:$1 ./build/cache_adaptive_balloon 0 $MATRIXWIDTH $TOTALMEMORY $1
    echo "Done with cache-adaptive constant memory"
		pkill -f balloon


  done
done

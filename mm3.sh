#!/bin/bash
set -x

NUMRUNS=1
SLEEP=5
declare -a matrixwidth=( 4096 )
declare -a startingmemory=( 32 )

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
    cgexec -g memory:$1 ./build/non_cache_adaptive 0 $MATRIXWIDTH $STARTINGMEMORY $1
    echo "Done with cache-adaptive constant memory"
	done
done

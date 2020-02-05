#!/bin/bash
set -ex

NUMRUNS=1
SLEEP=5
declare -a matrixwidth=( 2048  4096  )
declare -a startingmemory=( 10  10 )

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
    #run non-cache-adaptive on random memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    cgexec -g memory:$1 ./build/non_cache_adaptive 2 $MATRIXWIDTH $STARTINGMEMORY $1 > log1.txt &
    PID=$!
    STATUS=$(ps ax|grep "$PID"|wc -l)
    CURRENT=($STARTINGMEMORY*1024*1024)
    while [ $STATUS -gt 1 ]; do
      sleep $SLEEP
      if (( $RANDOM % 2 == 1 )) || (( $CURRENT == $STARTINGMEMORY*1024*1024 ));
      then
        echo "Increasing memory"
        let CURRENT=CURRENT+STARTINGMEMORY*1024*1024
      else
        echo "decreasing memory"
        let CURRENT=CURRENT-STARTINGMEMORY*1024*1024
      fi
      echo $CURRENT
      echo $CURRENT > /var/cgroups/$1/memory.limit_in_bytes
      STATUS=$(ps ax|grep "$PID"|wc -l)
    done
    echo "Done with non-cache-adaptive random memory"

    #run cache-adaptive on random memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    cgexec -g memory:$1 ./build/cache_adaptive 2 $MATRIXWIDTH $STARTINGMEMORY $1 > log1.txt &
    PID=$!
    STATUS=$(ps ax|grep "$PID"|wc -l)
    CURRENT=($STARTINGMEMORY*1024*1024)
    while [ $STATUS -gt 1 ]; do
      sleep $SLEEP
      if (( $RANDOM % 2 == 1 )) || (( $CURRENT == $STARTINGMEMORY*1024*1024 ))  ;
      then
        echo "Increasing memory"
        let CURRENT=CURRENT+STARTINGMEMORY*1024*1024
      else
        echo "decreasing memory"
        let CURRENT=CURRENT-STARTINGMEMORY*1024*1024
      fi
      echo $CURRENT
      echo $CURRENT > /var/cgroups/$1/memory.limit_in_bytes
      STATUS=$(ps ax|grep "$PID"|wc -l)
    done
    echo "Done with cache-adaptive random memory"


    #run non-cache-adaptive on constant memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    cgexec -g memory:$1 ./build/non_cache_adaptive 0 $MATRIXWIDTH $STARTINGMEMORY $1
    echo "Done with non-cache-adaptive constant memory"

    #run cache-adaptive on constant memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    cgexec -g memory:$1 ./build/cache_adaptive 0 $MATRIXWIDTH $STARTINGMEMORY $1
    echo "Done with cache-adaptive constant memory"



    #run non-cache-adaptive on worst-case memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    cgexec -g memory:$1 ./build/non_cache_adaptive 1 $MATRIXWIDTH $STARTINGMEMORY $1
    echo "Done with non-cache-adaptive worst-case memory"

    #run cache-adaptive on worst-case memory
    ./cgroups.sh $1
    ./build/mm_data $MATRIXWIDTH
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
    IFS=$'\r\n ' GLOBIGNORE='*' command eval  'XYZ=($(cat mem_profile"$MATRIXWIDTH".txt))'
    cgexec -g memory:$1 ./build/cache_adaptive 1 $MATRIXWIDTH $STARTINGMEMORY $1 > log1.txt &
    PID=$!
    STATUS=$(ps ax|grep "$PID"|wc -l)
    X=0
    CURRENT=0
    while [ $STATUS -gt 1 ] && [ $X -lt "${#XYZ[@]}" ]; do
      VAL=${XYZ[$X]}
      sleep $(calc "($VAL - $CURRENT) /1000")
      CURRENT=$VAL
      #bash -c "sleep $DIFF"
      let X+=1
      #echo 10000000 > /var/cgroups/$3/memory.limit_in_bytes
      echo $((XYZ[X])) > /var/cgroups/$1/memory.limit_in_bytes
      let X+=1
      STATUS=$(ps ax|grep "$PID"|wc -l)
    done
    echo "Done with cache-adaptive worst-case memory"
  done
done

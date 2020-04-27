#!/bin/bash
set -ex

NUMRUNS=1
NUMBALLOONS=9
SLEEP=5
declare -a matrixwidth=( 2000 3000 4000 5000 6000 )
declare -a startingmemory=( 10 10 10 10 10  )
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
	for i in `seq 1 $NUMRUNS`;
  do
		echo $index
	  MATRIXWIDTH=${matrixwidth[$index]}
	  STARTINGMEMORY_MB=${startingmemory[$index]}
		#run cache-adaptive on constant
		echo "Running cache adaptive on constant memory"
		./cgroups.sh $1
		./build/mm_data $MATRIXWIDTH
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon2 > "balloon_log$j.txt" 0 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
			#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
		done
		cgexec -g memory:$1 ./build/cache_adaptive_balloon 0 $MATRIXWIDTH $STARTINGMEMORY_MB $1
		pkill -f balloon2 --signal SIGTERM
		sleep 5

		#run non-cache-adaptive on constant
		echo "Running non-cache adaptive on constant memory"
		./cgroups.sh $1
		./build/mm_data $MATRIXWIDTH
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon2 > "balloon_log$j.txt" 0 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
			#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
		done
		cgexec -g memory:$1 ./build/non_cache_adaptive_balloon 0 $MATRIXWIDTH $STARTINGMEMORY_MB $1
		pkill -f balloon2 --signal SIGTERM
		sleep 5

		#run non-cache-adaptive on worst-case
		echo "Running non-cache adaptive on worst-case memory"
		./cgroups.sh $1
		./build/mm_data $MATRIXWIDTH
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon2 > "balloon_log$j.txt" 3 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
			#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
		done
		cgexec -g memory:$1 ./build/non_cache_adaptive_balloon 1 $MATRIXWIDTH $STARTINGMEMORY_MB $1
		pkill -f balloon2 --signal SIGTERM
		sleep 5

		#run non-cache-adaptive on worst-case but replay
		echo "Running non cache adaptive on worst case but replay"
		./cgroups.sh $1
		./build/mm_data $MATRIXWIDTH
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon2 > "balloon_log$j.txt" 1 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
			#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
		done
		cgexec -g memory:$1 ./build/non_cache_adaptive_balloon 3 $MATRIXWIDTH $STARTINGMEMORY_MB $1
		pkill -f balloon2 --signal SIGTERM
		sleep 5

		#run cache-adaptive on worst-case
		echo "Running cache adaptive on worst-case memory"
		./cgroups.sh $1
		./build/mm_data $MATRIXWIDTH
		sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches; echo 0 > /proc/sys/vm/vfs_cache_pressure"
		echo $TOTALMEMORY > /var/cgroups/$1/memory.limit_in_bytes
		for j in `seq 1 $NUMBALLOONS`;
		do
			cgexec -g memory:$1 ./build/balloon2 > "balloon_log$j.txt" 1 $TOTALMEMORY_MB $STARTINGMEMORY_MB $NUMBALLOONS $j $MATRIXWIDTH &
			#cgexec -g memory:$1 ./build/balloon3 > "balloon_log.txt" $j &
		done
		cgexec -g memory:$1 ./build/cache_adaptive_balloon 1 $MATRIXWIDTH $STARTINGMEMORY_MB $1
		pkill -f balloon2 --signal SIGTERM
		sleep 5
	done
done

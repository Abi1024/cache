#!/bin/bash

#This script takes "memory-available database-name profile-number timer-interval program" as input
args=("$@")

if [ $# -ne 5 ]; then
	echo "USAGE: ./mem_profile.sh <starting-memory-in-MB> <database-name> <memory-profile-to-run> <timer-interval-for-profile> <program#>"
	echo "Memory Profile 1: No change to M, enter a dummy value for T"
	echo "Memory Profile 2: Reduce M in half after T seconds"
	echo "Memory Profile 0: Set M to 48GB"
	echo "Memory Profile 3: Reduce M by half every T seconds until 100MB limit"
	echo "Memory Profile 4: Reduce M by half every T seconds until 100MB limit and then double it every T seconds until M is reached"
	echo "Memory Profile 5: Increase and decrease M at random every T seconds until sorting is complete"
	echo "Memory Profile 6: Reduce M to 100 MB after T seconds"
	echo "Memory Profile 7: Double M after T seconds"

	echo "Program 2: EM Mergesort"
	echo "Program 1: Funnel Sort"
	echo "Program 3: Quick Sort"

	exit
fi	

echo "Doing a pagecache writeback and flush...Switching to SU to do this"
sudo ../../../cgroups/flush_pagecache.sh

echo "Setting initial memory of cache-test cgroup to "${args[0]}" MB"
let M=${args[0]}*1024*1024
let M1=${args[0]}*1024*1024
echo $M > /var/cgroups/cache-test/memory.limit_in_bytes

if [ "${args[4]}" = "2" ]; then
	echo "Running EM Merge-Sort with STXXL... DB Selected: "${args[1]}
	nohup cgexec -g memory:cache-test merge_sort_strings ${args[0]} ../../../database_sample/DB_${args[1]} temp   >> results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]} &

elif [ "${args[4]}" = "1" ]; then
	echo "Running Funnel-Sort with STXXL... DB Selected: "${args[1]}
	nohup cgexec -g memory:cache-test ../funnel-sort-STXXL/build/local/sort-DB ../database_sample/DB_${args[1]} temp   >> results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]} &

elif [ "${args[4]}" = "3" ]; then
	echo "Running Quick-Sort with STXXL... DB Selected: "${args[1]}
	nohup cgexec -g memory:cache-test ../quick-sort-STXXL/build/local/quick-sort-DB ../database_sample/DB_${args[1]} temp   >> results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]} &
fi
PID=$! 

sar -A -o sar-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}.log 1 >/dev/null 2>&1 &
SARID=$!

if [ "${args[2]}" = "1" ]; then 
	echo "Running Memory Profile 1: No change to "${args[0]}", enter a dummy value for "${args[3]}>>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	let M=${args[0]}*1024*1024
	echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
	wait $PID

elif [ "${args[2]}" = "2" ]; then 
	echo "Running Memory Profile 2: Reduce "${args[0]}" in half after "${args[3]}" seconds">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	let M=${args[0]}*1024*512
	sleep ${args[3]}s
	echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
	wait $PID

elif [ "${args[2]}" = "0" ]; then
	echo "Running Memory Profile 0: Set M to 48GB">>results-48000-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	let M=48*1024*1024*1024
	echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
	wait $PID

elif [ "${args[2]}" = "3" ]; then
	echo "Running Memory Profile 3: Reduce "${args[0]}" by half every "${args[3]}" seconds until 100MB limit">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	STATUS=$(ps ax|grep "$PID"|wc -l)
	while [ $STATUS -gt 1 ] && [ $M -gt 104857600 ]; do
		let M/=2
		echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
		sleep ${args[3]}s
		STATUS=$(ps ax|grep "$PID"|wc -l)
	done
	wait $PID

elif [ "${args[2]}" = "4" ]; then
	echo "Running Memory Profile 4: Reduce "${args[0]}" by half every "${args[3]}" seconds until 100MB limit and then double it every "${args[3]}" seconds until "${args[0]}" is reached">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	STATUS=$(ps ax|grep "$PID"|wc -l)
	while [ $STATUS -gt 1 ] && [ $M -gt 104857600 ]; do
		let M/=2
		echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
		sleep ${args[3]}s
		STATUS=$(ps ax|grep "$PID"|wc -l)
	done
	while [ $STATUS -gt 1 ] && [ $M -lt $M1 ]; do
		let M*=2
		echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
		sleep ${args[3]}s
		STATUS=$(ps ax|grep "$PID"|wc -l)
	done
	wait $PID

elif [ "${args[2]}" = "5" ]; then
	echo "Running Memory Profile 5"
	echo "Running Memory Profile 5: Increase and decrease "${args[0]}" at random every "${args[3]}" seconds until sorting is complete">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	STATUS=$(ps ax|grep "PID"|wc -l)
	let LOWERBOUND=100*1024*1024
	let UPPERBOUND=$((M*2))
	let RANGE=$((M*2-LOWERBOUND))
	echo "UPPERBOUND: "$UPPERBOUND", LOWERBOUND: "$LOWERBOUND", RANGE: "$RANGE
	RANDOM=$$
	let NUMBER=$RANDOM*1024*1024
	let NEW_MEM=$((LOWERBOUND+NUMBER%RANGE))
	while [ $STATUS -gt 1 ]; do
		let NUMBER=$RANDOM*1024*1024
		let NEW_MEM=$((LOWERBOUND+NUMBER%RANGE))
		let M=$NEW_MEM
		echo "New memory: "$M
		echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
		sleep ${args[3]}s
		STATUS=$(ps ax|grep "$PID"|wc -l)
	done

elif [ "${args[2]}" = "6" ]; then 
	echo "Running Memory Profile 6: Reduce "${args[0]}" to 100 MB after "${args[3]}" seconds">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	let M=100*1024*1024
	sleep ${args[3]}s
	echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
	wait $PID

elif [ "${args[2]}" = "7" ]; then 
	echo "Running Memory Profile 7: Increase "${args[0]}" to double after "${args[3]}" seconds">>results-${args[0]}-${args[1]}-${args[2]}-${args[3]}-${args[4]}
	let M=${args[0]}*2*1024*1024
	sleep ${args[3]}s
	echo $M > /var/cgroups/cache-test/memory.limit_in_bytes
	wait $PID
fi

kill -9 $SARID
echo "Restoring original cache setting"
sudo sh -c "echo 100 > /proc/sys/vm/vfs_cache_pressure"

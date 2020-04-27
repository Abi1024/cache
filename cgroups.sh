#!/bin/bash
set -ex

if [ $# -ne 1 ]
then
	echo "Must supply 1 arguments. \n \
	Usage: sudo ./cgroups.sh <cgroup_name "
fi

if [ -d  "/var/cgroups/$1" ]
then
	sudo cat "/var/cgroups/$1/tasks"
	cgdelete memory:$1
fi

cgcreate -g "memory:$1" -t abiyaz:abiyaz
sudo bash -c "echo 1 > /var/cgroups/$1/memory.oom_control"

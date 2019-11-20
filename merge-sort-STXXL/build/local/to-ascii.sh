#!/bin/bash

for filename in sar-*.log
do
	echo "Processing file: $filename"
	LC_ALL=C;sar -f $filename | egrep  '[0-9][0-9]:[0-9][0-9]:[0-9][0-9]'    | sed  '1s/^/#/' >IOWait-$filename.ascii
	LC_ALL=C;sar -d -f $filename | egrep  '[0-9][0-9]:[0-9][0-9]:[0-9][0-9]'    | sed  '1s/^/#/' >Disk-$filename.ascii
	LC_ALL=C;sar -B -f $filename | egrep  '[0-9][0-9]:[0-9][0-9]:[0-9][0-9]'    | sed  '1s/^/#/' >Page-$filename.ascii
done


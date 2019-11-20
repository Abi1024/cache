#!/bin/bash
for filename in IOWait*2.log.ascii
do
	echo "Processing file: $filename"
	IOWaitmerge="${filename:0:-12}-2.log.ascii"
	IOWaitoutput="IOWait-plot-${filename:7:-12}"
	echo "Merge Sort Data File: $IOWaitmerge, Output Plot: $IOWaitoutput"
	gnuplot -e "filename_input='$IOWaitmerge'; filename_output='$IOWaitoutput'" iowait-plot.p
done
for filename in Disk*2.log.ascii
do
	echo "Processing file: $filename"
	Diskmerge="${filename:0:-12}-2.log.ascii"
	Diskoutput="Disk-plot-${filename:7:-12}"
	echo "Merge Sort Data File: $Diskmerge, Output Plot: $Diskoutput"
	gnuplot -e "filename_input='$Diskmerge'; filename_output='$Diskoutput'" disk-plot.p
done
for filename in Page*2.log.ascii
do
	echo "Processing file: $filename"
	Pagemerge="${filename:0:-12}-2.log.ascii"
	Pageoutput="Page-plot-${filename:7:-12}"
	echo "Merge Sort Data File: $Pagemerge, Output Plot: $Pageoutput"
	gnuplot -e "filename_input='$Pagemerge'; filename_output='$Pageoutput'" page-plot.p
done

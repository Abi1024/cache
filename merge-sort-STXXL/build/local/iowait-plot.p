set datafile commentschars "#%"
set terminal postscript
pdfcommand = "| ps2pdf - ".filename_output.".pdf"
set output pdfcommand
set title 'IOWait% vs Time Plot'
#set xdata time
#set timefmt '%H:%M:%S'
set xlabel 'Elapsed Time (s)'
set ylabel 'IOWait %'
set style line 1 lc rgb '#0025ad' lt 1 lw 1.5 
set style line 2  lc rgb '#09ad00' lt 1 lw 1.5
plot filename_input using 0:6 title 'EMMergeSort' w lines ls 2

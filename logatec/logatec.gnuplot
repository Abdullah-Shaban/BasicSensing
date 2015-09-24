set pm3d map
set xlabel "frequency [MHz]"
set ylabel "time [s]"
set cblabel "power [dBm]"
unset key
splot "logatec.csv" using ($2/1e6):1:3

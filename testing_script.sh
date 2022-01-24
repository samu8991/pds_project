#!/usr/bin/bash


###########################################################
#                       SIMULATION                        #  
###########################################################
benchmark_dir=~/benchmarks
build_dir=~/CLionProjects/pds_project/cmake-build-debug
cd $benchmark_dir && ls | grep 'rgg_n_2_1' >$build_dir/graphs.txt
cd $build_dir
rm time_values.dat
rm space_values.dat
i=1
while read -r g; do
    echo "Simulation number: $i" 
    runlim -o analisi.txt ./pds_project 2 3 4 $g 
    sed -i -e 's/\[runlim\]//g' analisi.txt
    while read -r descr val; do
        [[ $descr == 'time:' ]] && echo $val | sed 's/seconds//g'&& echo "$i $val">> time_values.dat
        [[ $descr == 'space:' ]] && echo $val | sed 's/MB//g' && echo "$i $val" >> space_values.dat
    done < analisi.txt
    echo ""
    let i=$i+1
done < graphs.txt
##########################################################
#                     PLOT RESULTS                       #
##########################################################
gnuplot -persist <<-EOFMarker
    set terminal png
    set output 'JonesPlassmanTime.png'
    set title "Jones Plassmann time simulation";

    set xlabel "Iterations"
    set ylabel "Time(seconds)"
    set grid 
    plot "time_values.dat" with lines lw 3

    pause -1 "Hit Enter to continue" 
     
EOFMarker
gnuplot -persist <<-EOFMarker
    set terminal png
    set output 'JonesPlassmanSpace.png'
    set title "Jones Plassmann space occupation simulation";

    set xlabel "Iterations"
    set ylabel "Space(MB)"
    set grid 
    plot "space_values.dat" with lines lw 3

    pause -1 "Hit Enter to continue" 
     
EOFMarker
[[ ! -d "~/CLionProjects/pds_project/cmake-build-debug/test_immages" ]] && mkdir test_immages

mv *.png test_immages
#
exit 0

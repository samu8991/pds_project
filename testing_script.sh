#!/usr/bin/bash
###########################################################
#                       INPUT HANDLING                    #
###########################################################
if [ $# -ne 3 ]; then
    echo "Arguments are: rappresentation, algorithm, number of threads"
    exit -2
fi

case $1 in
    0)r="Adjacency List";;
    1)r="Adjacency Matrix";;
    2)r="Compressed Sparse Row";;
    *)
        echo "Wrong choice! rappresentation goes from 0 to 2"
        exit -3;;
esac
case $2 in
    0)a="Sequential";;
    1)a="Parallel Sequential";;
    2)a="Luby";;
    3)a="Jones Plassman";;
    *)
        echo "Wrong choice! algorithms goes from 0 to 3"
        exit -4;;
esac

case $3 in
    0) t="hw";;
    1) t=$3;;
    2) t=$3;;
    4) t=$3;;
    8) t=$3;;
    *) exit -5;;
esac
###########################################################
#                       FUNCTIONS                         #
###########################################################
print_value_sofar(){
    echo "Time Excedeed"
    echo "Now I will plot results where the program stopped"
gnuplot -persist <<-EOFMarker
    set terminal png
    set output '${a}Time($t)threads[$r]TimeExceeded.png'
    set title "$a time simulation($t threads)[$r]TimeExceeded";

    set xlabel "Number of nodes(2^x)"
    set ylabel "Time(seconds)"
    set grid 
    set xtics 1
    plot "time_values.dat" with lines lw 3

     
EOFMarker
gnuplot -persist <<-EOFMarker
    set terminal png
    set output '${a}Space($t)threads[$r]TimeExceeded.png'
    set title "$a space occupation simulation($t threads)[$r]TimeExceeded";

    set xlabel "Number of nodes(2^x)"
    set ylabel "Space(MB)"
    set grid 
    set xtics 1
    plot "space_values.dat" with lines lw 3

    pause -1 "Hit Enter to continue" 
     
EOFMarker
mv *.png "test_immages/$a"
    exit -5        
}         

###########################################################
#                       SIMULATION                        #  
###########################################################
benchmark_dir=~/benchmarks
build_dir=~/CLionProjects/pds_project/cmake-build-debug
cd $benchmark_dir && ls | grep 'rgg_n_2_[1-2][0|5-9|1-2]'>$build_dir/graphs.txt && cd $build_dir
if [ -f time_values.dat ]
    then
        rm time_values.dat 
fi

if [ -f space_values.dat ]
    then
        rm space_values.dat
fi
i=1
while read -r g; do
    echo "Simulation number: $i" 
    runlim -o analisi.txt -r 600 ./pds_project $1 $2 $3 $g || print_value_sofar #time limit a 10 min 
    sed -i -e 's/\[runlim\]//g' analisi.txt
    let i=$i+14
    while read -r descr val; do
        [[ $descr == 'time:' ]] && val=$(echo $val | sed 's/seconds//g')&& echo "$i $val">> time_values.dat
        [[ $descr == 'space:' ]] && val=$(echo $val | sed 's/MB//g') && echo "$i $val" >> space_values.dat
    done < analisi.txt
    echo ""
    let i=$i+1-14
done < graphs.txt
##########################################################
#                     PLOT RESULTS                       #
##########################################################
gnuplot -persist <<-EOFMarker
    set terminal png
    set output '${a}Time($t)threads[$r].png'
    set title "$a time simulation($t threads)[$r]";

    set xlabel "Number of nodes(2^x)"
    set ylabel "Time(seconds)"
    set grid 
    set xtics 1
    plot "time_values.dat" with lines lw 3

     
EOFMarker
gnuplot -persist <<-EOFMarker
    set terminal png
    set output '${a}Space($t)threads[$r].png'
    set title "$a space occupation simulation($t threads)[$r]";

    set xlabel "Number of nodes(2^x)"
    set ylabel "Space(MB)"
    set grid 
    set xtics 1
    plot "space_values.dat" with lines lw 3

    pause -1 "Hit Enter to continue" 
     
EOFMarker
if [ ! -d "$build_dir/test_immages" ]; then 
    mkdir test_immages
fi

mv *.png "test_immages/$a"

exit 0

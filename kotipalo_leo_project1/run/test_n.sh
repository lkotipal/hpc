#!/bin/bash

ntasks=4
for i in 1 2 3 4 5 6 7 8 9 10 20
do 
	n=$((10*i))
	echo $n
	for j in {0..99}
	do
		/usr/bin/time -a -o "times_${ntasks}_${n}.tsv" -f "%e" mpirun -np ${ntasks} test.exe ${n} 1>> out_${ntasks}_${n}.tsv 2>> /dev/null
	done
done
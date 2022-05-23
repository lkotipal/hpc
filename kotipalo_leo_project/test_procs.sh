#!/bin/bash

n=100
for ntasks in 1 2 8 16
do 
	echo $ntasks
	for j in {0..99}
	do
		/usr/bin/time -a -o "times_${ntasks}_${n}.tsv" -f "%e" mpirun -np ${ntasks} test.exe $n 1>> out_${ntasks}_${n}.tsv 2>> /dev/null
	done
done
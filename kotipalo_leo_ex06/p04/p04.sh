#!/bin/bash

for i in {0..50}
do
	mpirun -np 2 mpiexample.exe `expr 2000000 \* $i` > times_$i.tsv
done
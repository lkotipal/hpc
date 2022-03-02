#!/bin/bash

x=1
for i in {0..28}
do
	mpirun -np 2 mpiexample.exe $x > p04_$x.tsv
	x=`expr 2 \* $x`
done
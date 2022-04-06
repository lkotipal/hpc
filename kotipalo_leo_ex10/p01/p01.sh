#!/bin/bash

for i in {0..99}
do
    /usr/bin/time -a -o "times_$1.tsv" -f "%e" mpirun -np $1 md1d.exe 8000 0.01 10000 1 1000 > /dev/null
done
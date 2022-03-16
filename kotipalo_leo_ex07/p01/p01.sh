#!/bin/bash

for i in {0..99}
do
	mpirun -np $1 mpiexample.exe
done
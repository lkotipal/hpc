#!/bin/bash

for i in {0..100}
do
	mpirun -np $1 mpiexample.exe
done
#!/bin/bash

#SBATCH -M ukko2             # cluster (you may also use --cluster=ukko2)
#SBATCH -e out_%j            # stderr
#SBATCH -o out_%j            # stdout
#SBATCH --mem-per-cpu=300    # memory
#SBATCH -t 00:01:00          # time needed dd-hh:mm:ss
#SBATCH -n 2                 # number of cores
#SBATCH -p test             # queue

module purge
module load gnu9
module load openmpi4

echo -e "\n=== Run started $(date) on $HOSTNAME ===\n"

mpirun -np 2 ./mpiexample.exe testing1 testing2 &> mpiexample.out

echo -e "\n=== Run finished $(date) $HOSTNAME ===\n"

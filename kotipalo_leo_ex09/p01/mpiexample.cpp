#include <iostream>
#include <mpi.h>
#include "matrix.hpp"

int main(int argc,char *argv[])
{
	const int tag=50;
	int id, ntasks, rc;
	MPI_Status status;

	if (MPI_Init(&argc,&argv) != MPI_SUCCESS) {
		std::clog << "MPI initialization failed" << std::endl;
		exit(1);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	Matrix m(ntasks);
	for (int i = 0; i < ntasks; ++i)
		m(i, i) = id;

	MPI_Datatype diag_type;
	MPI_Type_vector(ntasks, 1, ntasks + 1, MPI_INT, &diag_type);
	MPI_Type_commit(&diag_type);
	MPI_Gather(m.data(), 1, diag_type, m.data(), ntasks, MPI_INT, 0, MPI_COMM_WORLD);

	if (id == 0)
		std::cout << m;

	MPI_Finalize();
	exit(0);
}

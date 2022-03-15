#include <iostream>
#include <memory>
#include <mpi.h>

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

	int msg{id};
	if (id) {
		for (int i = 0; i < 100; ++i)
			MPI_Send(&msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	} else {
		for (int i = 0; i < (ntasks-1)*100; ++i) {
			MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			std::cout << msg << std::endl;
		}
	}

	MPI_Finalize();
	exit(0);
}

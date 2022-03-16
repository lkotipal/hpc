#include <cmath>
#include <iostream>
#include <mpi.h>
#include <vector>

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

	if (id == 0) {
		std::cout << "Running " << ntasks << " processes" << std::endl;
	}

	int msg{id};
	int buf{0};
	if (id == 0) {
		// Root receives from all powers of 2
		for (int i = 1; i < ntasks; i *= 2) {
			MPI_Recv(&buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			msg += buf;
		}

		std::cout << msg << std::endl;
	} else if (id % 2) {
		// Odd processes send to previous process
		MPI_Send(&msg, 1, MPI_INT, id - 1, tag, MPI_COMM_WORLD);
	} else {
		// Even processes receive from next process, if they're not the last one
		if (id < ntasks - 1)
			MPI_Recv(&buf, 1, MPI_INT, id + 1, tag, MPI_COMM_WORLD, &status);
		msg += buf;

 		if (id & id - 1) {
			// If not power of 2, send to previous one
			MPI_Send(&msg, 1, MPI_INT, std::exp2(static_cast<int>(std::log2(id))), tag, MPI_COMM_WORLD);
		} else {
			// If power of 2, receive from even numbers and send to root
			for (int i = id + 2; i < id * 2 && i < ntasks; i += 2) {
				MPI_Recv(&buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
				msg += buf;
			}
			MPI_Send(&msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	exit(0);
}

#include <iostream>
#include <mpi.h>

int main(int argc,char *argv[])
{
	const int tag=50;
	int id, ntasks, rc, i, pnlen;
	MPI_Status status;
	int* msg = new int{0};
	char pname[MPI_MAX_PROCESSOR_NAME];

	rc=MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
		std::clog << "MPI initialization failed" << std::endl;
		exit(1);
	}
	rc=MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	rc=MPI_Comm_rank(MPI_COMM_WORLD,&id);
	rc=MPI_Get_processor_name(pname,&pnlen);

	if (id == 0) {
		std::cout << id << ": " << (*msg) << std::endl;
		rc=MPI_Send(msg, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
		rc=MPI_Recv(msg, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
		std::cout << id << ": " << ++(*msg) << std::endl;
		rc=MPI_Send(msg, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
		rc=MPI_Recv(msg, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
		std::cout << id << ": " << ++(*msg) << std::endl;
	} else if (id == 1) {
		rc=MPI_Recv(msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		std::cout << id << ": " << ++(*msg) << std::endl;
		rc=MPI_Send(msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		rc=MPI_Recv(msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		std::cout << id << ": " << ++(*msg) << std::endl;
		rc=MPI_Send(msg, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}

	rc=MPI_Finalize();
	exit(0);
}

#include <iostream>
#include <mpi.h>

int main(int argc,char *argv[])
{
	const int tag=50;
	int id, ntasks, rc, pnlen;
	MPI_Status status;
	char pname[MPI_MAX_PROCESSOR_NAME];

	rc=MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
		std::clog << "MPI initialization failed" << std::endl;
		exit(1);
	}
	rc=MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	rc=MPI_Comm_rank(MPI_COMM_WORLD,&id);
	rc=MPI_Get_processor_name(pname,&pnlen);

	if (id < ntasks - 1) {
		int msg[1] = {id};
		std::cout << id << " sending " << msg[0] << " to " << id+1 << std::endl;
		rc=MPI_Send(msg, 1, MPI_INT, id+1, tag, MPI_COMM_WORLD);
	}

	if (id > 0) {
		int msg[1];
		rc=MPI_Recv(msg, 1, MPI_INT, id-1, tag, MPI_COMM_WORLD, &status);
		int source_id=status.MPI_SOURCE;
		std::cout << id << " receiving " << msg[0] << " from " << source_id << std::endl;
	}

	rc=MPI_Finalize();
	exit(0);
}

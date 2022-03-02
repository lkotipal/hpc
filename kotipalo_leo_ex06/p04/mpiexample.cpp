#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc,char *argv[])
{
	constexpr int tag=50;
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

	int n = 0;
	if (argc > 1) {
		try {
			n = std::stoi(argv[1]);
			if (n < 0) {
				if (id == 0)
					std::clog << "n must be a positive integer." << std::endl;
				n = 0;
			}
		} catch (std::invalid_argument e) {
			if (id == 0)
				std::clog << "n must be a positive integer." << std::endl;
		}
	}
	if (id == 0) {
		std::clog << "Using n = " << n << std::endl;
		std::clog << "Message size " << n * sizeof(char) << std::endl << std::endl;
	}

	int other_id = id ? 0 : 1;
	std::vector<char> msg(n); 
	double start, stop;
	if (id == 0) {
		start = MPI_Wtime();
		rc=MPI_Send(msg.data(), n, MPI_CHAR, other_id, tag, MPI_COMM_WORLD);
	}

	std::cout << std::scientific;
	if (id < 2) {
		constexpr int loops = 1'000;
		for (int i = 0; (id && i < loops) || (id == 0 && i < loops - 1); ++i) {
			rc=MPI_Recv(msg.data(), n, MPI_CHAR, other_id, tag, MPI_COMM_WORLD, &status);
			if (id == 0) {
				stop = MPI_Wtime();
				std::cout << stop - start << std::endl;
				start = stop;
			}
			rc=MPI_Send(msg.data(), n, MPI_CHAR, other_id, tag, MPI_COMM_WORLD);
		}
	}

	if (id == 0) {
		rc=MPI_Recv(msg.data(), n, MPI_CHAR, other_id, tag, MPI_COMM_WORLD, &status);
		stop = MPI_Wtime();
		std::cout << stop - start << std::endl;
	}

	rc=MPI_Finalize();
	exit(0);
}

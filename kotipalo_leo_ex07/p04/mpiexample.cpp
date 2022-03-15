#include <iostream>
#include <memory>
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

	if (ntasks & ntasks - 1) {
		std::clog << "ntasks must be power of 2" << std::endl;
		MPI_Finalize();
		exit(0);
	}

	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int msg{id};
	for (int i = ntasks-1; i >= 0; --i) {
		if (i % 2 == 0) {
			// Only even IDs gather
			int count_send{0};
			int count_receive{0};
			std::vector<int> count_recv(id == i ? ntasks : 0);
			std::vector<int> displacements(id == i ? ntasks : 0);
			if (i == 0) {
				// Root, gather from all powers of 2
				if (id == i) {
					for (int j = 1; j < ntasks; j *= 2) {
						count_recv[j] = 1;
						displacements[j] = count_receive++;
					}
				} else if ((id & id - 1) == 0) {
					count_send = 1;
				}
			} else {
				// Gather from next number
				if (id == i) {
					count_recv[i+1] = 1;
					displacements[i+1] = count_receive++;
				} else if (id == i + 1) {
					count_send = 1;
				}

				if ((i & i - 1) == 0) {
					// Power of 2, gather from all even numbers up to next power
					if (id == i) {
						for (int j = i + 2; j < i * 2; j += 2) {
							count_recv[j] = 1;
							displacements[j] = count_receive++;
						}
					} else if (id % 2 == 0 && id > i && id < 2 * i) {
						count_send = 1;
					}
				}
			}
			if (count_receive)
				std::cout << i << " receiving " << count_receive << std::endl;
			if (count_send)
				std::cout << id << " sending " << msg << " to " << i << std::endl;
			std::vector<int> buffer_recv(count_receive);
			MPI_Gatherv(&msg, count_send, MPI_INT, buffer_recv.data(), count_recv.data(), displacements.data(), MPI_INT, i, MPI_COMM_WORLD);

			if (id == i)
				for (int j = 0; j < buffer_recv.size(); ++j)
					msg += buffer_recv[j];
		}
	}

	if (id == 0)
		std::cout << msg << std::endl;

	MPI_Finalize();
	exit(0);
}

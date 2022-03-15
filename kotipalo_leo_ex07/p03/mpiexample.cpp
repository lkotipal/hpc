#include <fstream>
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
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	int n{0};
	std::vector<double> file_nums;
	std::vector<int> counts(ntasks);
	std::vector<int> displacements(ntasks);
	if (id == 0) {
		std::ifstream f("ex7p3.dat");
		if (f.is_open()) {
			double d{0.0};
			while (f >> d)
				file_nums.push_back(d);
		}

		for (int i = 0; i < ntasks; ++i)
			counts[i] = i < file_nums.size() % ntasks ? file_nums.size() / ntasks + 1 : file_nums.size() / ntasks;
		for (int i = 1; i < ntasks; ++i)
			displacements[i] = displacements[i-1] + counts[i-1];
	}

	int count{0};
	MPI_Scatter(counts.data(), 1, MPI_INT, &count, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<double> nums(count);
	MPI_Scatterv(file_nums.data(), counts.data(), displacements.data(), MPI_DOUBLE, nums.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	double sum{0.0};
	double square_sum{0.0};
	for (double d : nums) {
		sum += d;
		square_sum += d * d;
	}

	std::vector<double> sums(id ? 0 : ntasks);
	std::vector<double> square_sums(id ? 0 : ntasks);
	MPI_Gather(&sum, 1, MPI_DOUBLE, sums.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(&square_sum, 1, MPI_DOUBLE, square_sums.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (id == 0) {
		for (int i = 1; i < ntasks; ++i) {
			sum += sums[i];
			square_sum += square_sums[i];
		}

		double mean = sum / file_nums.size();
		std::cout << "Average:\t" << mean << std::endl;
		std::cout << "Variance:\t" << square_sum / file_nums.size() - mean * mean << std::endl;
	}

	MPI_Finalize();
	exit(0);
}

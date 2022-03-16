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

	int my_n{0};
	MPI_Scatter(counts.data(), 1, MPI_INT, &my_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<double> my_nums(my_n);
	MPI_Scatterv(file_nums.data(), counts.data(), displacements.data(), MPI_DOUBLE, my_nums.data(), my_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	double my_mean{0.0};
	double my_square_sum{0.0};
	for (int i = 0; i < my_n; ++i) {
		double temp = my_mean;
		my_mean += (my_nums[i] - my_mean) / (i+1);
		my_square_sum += (my_nums[i] - temp) * (my_nums[i] - my_mean);
	}

	std::vector<double> means(id ? 0 : ntasks);
	std::vector<double> square_sums(id ? 0 : ntasks);
	MPI_Gather(&my_mean, 1, MPI_DOUBLE, means.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(&my_square_sum, 1, MPI_DOUBLE, square_sums.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (id == 0) {
		for (int i = 1; i < ntasks; ++i) {
			double delta = means[i] - my_mean;
			my_mean += delta * counts[i] / (my_n + counts[i]);
			my_square_sum += square_sums[i] + delta * delta * (my_n * counts[i]) / (my_n + counts[i]);
			my_n += counts[i];
		}

		std::cout << "Average:\t" << my_mean << std::endl;
		std::cout << "Variance:\t" << my_square_sum / my_n << std::endl; // Population variance, without Bessel correction
	}

	MPI_Finalize();
	exit(0);
}

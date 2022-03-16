#include <cmath>
#include <iostream>
#include <mpi.h>
#include <vector>
#include "rejection_sampler.hpp"

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

	constexpr int N = 1'000'000;
	Rejection_sampler rs{static_cast<std::uint_fast32_t>(std::pow(7, id))};

	double p = rs.integrate(N);
	std::vector<double> ps(id ? 0 : ntasks);
	MPI_Gather(&p, 1, MPI_DOUBLE, ps.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (id == 0) {
		for (int i = 1; i < ntasks; ++i)
			p += ps[i];
		p /= ntasks;
		std::cout << "π = " << 4 * p << " +- " << 4 * p * std::sqrt((p - p*p) / (N * ntasks)) << std::endl;
	}

	MPI_Finalize();
	exit(0);
}

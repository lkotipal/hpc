#include <cerrno>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <numbers>
#include "point.hpp"
#include "salesman.hpp"

int main(int argc, char *argv[]) 
{
	if (MPI_Init(&argc,&argv) != MPI_SUCCESS) {
		std::clog << "MPI initialization failed" << std::endl;
		exit(1);
	}

	int id;
	int ntasks;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

	std::string in_file;
	if (argc > 1) {
		in_file = argv[1];
		if (id == 0)
			std::clog << "Using file " << in_file << " as input." << std::endl;
	} else {
		if (id == 0)
			std::clog << "Usage: mpirun -np NTASKS salesman.exe INPUT (SEED)" << std::endl;
		MPI_Finalize();
		return 0;
	}

	std::uint_fast32_t seed;
	if (id == 0) {
		if (argc > 2) {
			try {
				seed = std::stoi(argv[2]);
			} catch (std::invalid_argument e) {
				std::clog << "Invalid seed given." << std::endl;
			}
		} else {
			std::clog << "No seed given." << std::endl;
			std::random_device r;
			seed = r();
		}
		std::clog << "Using seed " << seed << std::endl;
	}
	MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD)	;

	std::mt19937 rng {seed};
	std::uniform_real_distribution<double> u {0, 1};
	std::uniform_real_distribution<double> theta {0, 2 * std::numbers::pi};
	constexpr int generations {10};

	std::vector<double> x_vec;
	std::vector<double> y_vec;
	int n_cities {0};
	if (id == 0) {
		std::ifstream f {in_file};
		if (f.fail()) {
			std::cerr << "Could not read " << in_file << ": " << std::strerror(errno) << std::endl;
			MPI_Abort(MPI_COMM_WORLD, errno);
		}
		double x, y;
		while (f >> x >> y) {
			x_vec.push_back(x);
			y_vec.push_back(y);
			++n_cities;
		}
		f.close();
	}


	MPI_Bcast(&n_cities, 1, MPI_INT, 0, MPI_COMM_WORLD)	;
	x_vec.resize(n_cities);
	y_vec.resize(n_cities);

	MPI_Bcast(x_vec.data(), n_cities, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(y_vec.data(), n_cities, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	std::vector<Point> cities;
	cities.reserve(n_cities);
	for (int i = 0; i < n_cities; ++i)
		cities.push_back(std::array<double, 2> {x_vec[i], y_vec[i]});
	
	const int population {100 * n_cities / ntasks};
	Salesman sm {cities, population, seed + id};

	// Best route always ends up in process 0 before termination, so no need to reduce
	int gens = sm.simulate(generations);
	if (id == 0) {
		auto route = sm.best_route();
		std::cout << "Best route length " << route.get_length() << " found in " << gens << " generations." << std::endl;
		std::ofstream f {"route.tsv"};
		f << std::fixed << route;
		f.close();
	}

	MPI_Finalize();

	return 0;
}
#include <cmath>
#include <fstream>
#include <iostream>
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

	std::uint_fast32_t seed = 1;
	if (argc > 1) {
		try {
			seed = std::stoi(argv[1]);
		} catch (std::invalid_argument e) {
			if (id == 0)
				std::clog << "Invalid seed given." << std::endl;
		}
	} else if (id == 0) {
		std::clog << "No seed given." << std::endl;
	}
	if (id == 0)
		std::clog << "Using seed " << seed << std::endl;

	std::mt19937 rng {seed};
	std::uniform_real_distribution<double> u {0, 1};
	std::uniform_real_distribution<double> theta {0, 2 * std::numbers::pi};
	const int population {1'000};
	const int generations {10};

	std::vector<double> x_vec;
	std::vector<double> y_vec;
	int n_cities {0};
	if (id == 0) {
		std::ifstream f {"cities.dat"};
		while (!f.eof()) {
			double x, y;
			std::string name;
			f >> x >> y >> name;
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
	
	Salesman sm {cities, population, seed + id};

	// Best route always ends up in process 0 before termination, so no need to reduce
	int gens = sm.simulate(generations);
	if (id == 0) {
		auto route = sm.best_route();
		double l = sm.f(route);
		std::cout << "Best route length " << l << " found in " << gens << " generations." << std::endl;
		std::ofstream f {"route.tsv"};
		for (int j : route) {
			Point p = cities[j];
			f << std::fixed << p[0] << "\t" << p[1] << std::endl;
		}
		f.close();
	}

	MPI_Finalize();

	return 0;
}
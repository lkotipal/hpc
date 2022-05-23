#include <cmath>
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

	int vertex_count;
	if (argc > 1) {
		try {
			vertex_count = std::stoi(argv[1]);
		} catch (std::invalid_argument e) {
			if (id == 0)
				std::clog << "Invalid vertex count given." << std::endl;
		}
	} else {
		if (id == 0)
			std::clog << "Usage: mpirun -np NTASKS test.exe N_VERTEX (SEED)" << std::endl;
		MPI_Finalize();
		return 0;
	}

	std::uint_fast32_t seed = 1;
	if (argc > 2) {
		try {
			seed = std::stoi(argv[2]);
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
	const int population {1'000};			// Make sure population is divisible by 4, and scale by task count
	const int generations {ntasks};			// Ensure best route is migrated
	constexpr int trials {10};

	std::vector<Point> square_points;
	for (int i = 0; i < vertex_count; ++i)
		square_points.push_back(Point(std::array<double, 2>{u(rng), u(rng)}));

	std::vector<Point> circle_points;
	for(int i = 0; i < vertex_count; ++i)
		circle_points.push_back(Point(1, theta(rng)));

	std::array<std::vector<Point>, 2> point_vecs {square_points, circle_points}; 
	std::array<std::string, 2> names {"square", "circle"};
	
	for (int i = 0; i < 2; ++i) {
		std::ofstream f_lengths;
		if (id == 0) {
			f_lengths.open({names[i] + "_lengths.tsv"});
			f_lengths << "length" << "\t\t" << "generations" << std::endl;
		}

		auto points = point_vecs[i];
		Salesman sm {points, population, seed + id};

		double best_len = std::numeric_limits<double>::infinity();
		Route best_route(&point_vecs[i]);
		for (int j = 0; j < trials; ++j) {
			int gens = sm.simulate(generations);
			auto route = sm.best_route();
			double l = route.get_length();

			if (id == 0)
				f_lengths << std::fixed << l << "\t" << gens << std::endl;

			if (l < best_len) {
				best_len = l;
				best_route = route;
			}
		}

		// Best route always ends up in process 0 before termination, so no need to reduce
		if (id == 0) {
			std::cout << "Best route for " << names[i] << " length " << best_len << std::endl;
			std::ofstream f {names[i] + "_route.tsv"};
			f << std::fixed << best_route;
			f.close();
		}
	}

	MPI_Finalize();

	return 0;
}
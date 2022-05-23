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
	const int population {100 * vertex_count / ntasks};			// Make sure population is divisible by 4
	const int generations {10};
	constexpr int trials {100};

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
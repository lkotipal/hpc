#ifndef SALESMAN_HPP
#define SALESMAN_HPP
#include <random>
#include <vector>
#include "point.hpp"
#include "route.hpp"

// Class for solving the traveling salesman problem using a parallel genetic algorithm
class Salesman {
	public:
		// Constructor
		// Cities is the vector of cities to solve the problem for
		// Population is the population used in the genetic algorithm, per process
		// Seed is the seed for the random number generator
		Salesman(std::vector<Point>& cities, int population, std::uint_fast32_t seed) : 
			cities{cities}, routes(population, Route(&cities)), rng{seed} {}

		// Find a solution via genetic algorithm
		// Returns the generation where the solution was found
		// generations is the maximum amount of generations without improvement
		int simulate(int generations);

		// Returns the best route found by the algorithm as a vector of positions of cities in the route
		Route best_route() {return routes[0];}
	private:
		// Migrate 1/4 of the best routes so far to neighboring processes
		void communicate();

		// Create the next generation in the algorithm
		void evolve();

		// Sort routes by length
		void sort_routes();

		// Shuffle all routes
		void shuffle_routes();

		// Mersenne twister RNG
		std::mt19937 rng;
		std::uniform_real_distribution<double> u {0, 1};

		// Cities to solve the problem for
		std::vector<Point> cities;

		// Population of routes used in the genetic algorithm
		std::vector<Route> routes;
};

#endif

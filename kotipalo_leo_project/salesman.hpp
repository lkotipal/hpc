#ifndef SALESMAN_HPP
#define SALESMAN_HPP
#include <random>
#include <vector>
#include "point.hpp"

// Class for solving the traveling salesman problem using a parallel genetic algorithm
class Salesman {
	public:
		// Constructor
		// Cities is the vector of cities to solve the problem for
		// Population is the population used in the genetic algorithm, per process
		// Seed is the seed for the random number generator
		Salesman(const std::vector<Point>& cities, int population, std::uint_fast32_t seed);

		// Find a solution via genetic algorithm
		// Returns the generation where the solution was found
		// generations is the maximum amount of generations without improvement
		int simulate(int generations);

		// Returns the best route found by the algorithm as a vector of positions of cities in the route
		std::vector<int> best_route();

		// Returns the length of the route given
		double f(const std::vector<int>& route) const;
	private:
		// Migrate 1/4 of the best routes so far to neighboring processes
		void communicate();

		// Create the next generation in the algorithm
		void evolve();

		// Attempt to mutate given route by swapping two indices
		void mutate(std::vector<int>& route);

		// Cross over two routes given and return the child
		std::vector<int> crossover(std::vector<int>& first, std::vector<int>& second);

		// Sort routes by length
		void sort_routes();

		// Shuffle all routes
		void shuffle_routes();

		// Return the difference in length if indices i and j are swapped in given route
		double delta_f(const std::vector<int>& route, const int i, const int j) const;

		// Mersenne twister RNG
		std::mt19937 rng;
		std::uniform_real_distribution<double> u {0, 1};

		// Cities to solve the problem for
		std::vector<Point> cities;

		// Population of routes used in the genetic algorithm
		std::vector<std::vector<int>> routes;
};

#endif

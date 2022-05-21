#ifndef SALESMAN_HPP
#define SALESMAN_HPP
#include <algorithm>
#include <cassert>
#include <cmath>
#include <mpi.h>
#include <random>
#include <vector>
#include "point.hpp"

class Salesman {
	public:
		Salesman(const std::vector<Point>& cities, int population, std::uint_fast32_t seed);
		int simulate(int generations);
		std::vector<int> best_route();
		double f(const std::vector<int>& route) const;
	private:
		void communicate();
		void evolve();
		void mutate(std::vector<int>& route);
		std::vector<int> crossover(std::vector<int>& first, std::vector<int>& second);
		void sort_routes();
		void shuffle_routes();

		double delta_f(const std::vector<int>& route, const int i, const int j) const;

		double T;
		std::mt19937 rng;
		std::uniform_real_distribution<double> u {0, 1};
		std::vector<Point> cities;
		std::vector<std::vector<int>> routes;
};

inline Salesman::Salesman(const std::vector<Point>& cities, int population, std::uint_fast32_t seed) : cities{cities}, routes(population), rng{seed}
{
	for (int i = 0; i < cities.size(); ++i) { 
        for (int j = i + 1; j < cities.size(); ++j) {
            double dist = 2 * (cities[i] - cities[j]).norm();
            if (dist > T)
                T = dist;
        }
    }

	std::vector<int> route(cities.size());
	for (int i = 0; i < cities.size(); ++i)
		route[i] = i;

	for (int i = 0; i < population; ++i)
		routes[i] = route;

	sort_routes();
}

inline int Salesman::simulate(int generations)
{
	shuffle_routes();
	double len = f(routes[0]);
	int i {0};
	int last_improved {0};
	while (i - last_improved < generations) {
		++i;
		evolve();

		double new_len = f(routes[0]);
		if (new_len < len) {
			len = new_len;
			last_improved = i;
		}
		MPI_Allreduce(&last_improved, &last_improved, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	}
	return last_improved;
}

inline std::vector<int> Salesman::best_route()
{
	return routes[0];
}

inline double Salesman::f(const std::vector<int>& route) const
{
	double d {0.0};
	for (int i = 0; i < route.size() - 1; ++i)
		d += (cities[route[i]] - cities[route[i+1]]).norm();
	d += (cities[route[route.size() - 1]] - cities[route[0]]).norm();
	return d;
}

inline void Salesman::communicate()
{
	int id, ntasks;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int left = id ? id - 1 : ntasks - 1;
	int right = id < ntasks - 1 ? id + 1 : 0;
	
	for (int i = 0; i < routes.size() / 4; ++i) {
		MPI_Sendrecv(routes[i].data(), cities.size(), MPI_INT, left, 0, 
					 routes[routes.size() / 2 + i].data(), cities.size(), MPI_INT, right, 0, MPI_COMM_WORLD, &status);
		MPI_Sendrecv(routes[i].data(), cities.size(), MPI_INT, right, 0, 
					 routes[3 * routes.size() / 4 + i].data(), cities.size(), MPI_INT, left, 0, MPI_COMM_WORLD, &status);
	}
	sort_routes();
}

inline void Salesman::evolve()
{
	std::vector<std::vector<int>> best_routes(routes.begin(), routes.begin() + routes.size() / 2);
	std::uniform_int_distribution<int> idx(0, best_routes.size() - 1);
	for (int i = std::max(1, static_cast<int>(routes.size() / 8)); i < routes.size(); ++i) {
		routes[i] = crossover(best_routes[idx(rng)], best_routes[idx(rng)]);	// Self-pollination allowed
		mutate(routes[i]);
	}
	sort_routes();
	communicate();
}

inline void Salesman::mutate(std::vector<int>& route)
{
	// Don't move first point
	// Retains ergodicity as routes are unique up to starting point
	std::uniform_int_distribution<int> idx(1, route.size() - 1);
	int i {idx(rng)};
	int j;
	do {
		j = idx(rng);
	} while (j == i);

	//if (u(rng) < std::exp(-delta_f(route, i, j) / T));
	// Only accept good mutations for now
	if (delta_f(route, i, j) < 0)
	 	std::swap(route[i], route[j]);
}

inline std::vector<int> Salesman::crossover(std::vector<int>& first, std::vector<int>& second)
{
	assert(first.size() == second.size());
	std::vector<int> child(first.size());

	// First node is always first city
	auto first_it = first.begin() + 1;
	auto second_it = second.begin() + 1;
	child[0] = 0;
	for (auto it = child.begin() + 1; it < child.end(); ++it) {
		// Advance iterators until we find elements not in child yet
		while (std::find(child.begin(), it, *first_it) < it)
			++first_it;
		while (std::find(child.begin(), it, *second_it) < it)
			++second_it;
		
		// Pick next node as the one with the shorter distance to previous node
		*it = (cities[*first_it] - cities[*(it - 1)]).norm() < (cities[*second_it] - cities[*(it - 2)]).norm() ? *first_it : *second_it;
	}

	return child;
}

inline void Salesman::shuffle_routes()
{
	for (int i = 0; i < routes.size(); ++i) {
		std::shuffle(routes[i].begin() + 1, routes[i].end(), rng);
	}
	sort_routes();
}

inline void Salesman::sort_routes()
{
	std::sort(routes.begin(), routes.end(), [this](const std::vector<int>& a, const std::vector<int>& b) -> bool {
		return f(a) < f(b);
	});
}

// Extremely cursed
inline double Salesman::delta_f(const std::vector<int>& route, const int i, const int j) const
{
	int prev_i = i == 0 ? cities.size() - 1 : i - 1;
	int next_i = i == cities.size() - 1 ? 0 : i + 1;

	int prev_j = j == 0 ? cities.size() - 1 : j - 1;
	int next_j = j == cities.size() - 1 ? 0 : j + 1;

	if (i == prev_j)
		return (cities[route[i]] - cities[route[next_j]]).norm() + (cities[route[j]] - cities[route[prev_i]]).norm() -
			(cities[route[i]] - cities[route[prev_i]]).norm() - (cities[route[j]] - cities[route[next_j]]).norm();
	else if (i == next_j)
		return (cities[route[i]] - cities[route[prev_j]]).norm() + (cities[route[j]] - cities[route[next_i]]).norm() -
		(cities[route[i]] - cities[route[next_i]]).norm() - (cities[route[j]] - cities[route[prev_j]]).norm();
	else 
		return (cities[route[i]] - cities[route[prev_j]]).norm() + (cities[route[i]] - cities[route[next_j]]).norm() +
		(cities[route[j]] - cities[route[prev_i]]).norm() + (cities[route[j]] - cities[route[next_i]]).norm() -
		(cities[route[i]] - cities[route[prev_i]]).norm() - (cities[route[i]] - cities[route[next_i]]).norm() -
		(cities[route[j]] - cities[route[prev_j]]).norm() - (cities[route[j]] - cities[route[next_j]]).norm();
}

#endif

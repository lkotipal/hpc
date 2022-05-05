#ifndef SALESMAN_HPP
#define SALESMAN_HPP
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>
#include <vector>
#include "point.hpp"

class Salesman {
	public:
		Salesman(const std::vector<Point>& cities, int population, std::uint_fast32_t seed);
		void simulate(int generations);
		std::vector<int> best_route();
		double f(const std::vector<int>& route) const;
	private:
		void evolve();
		void mutate(std::vector<int>& route);
		std::vector<int> crossover(std::vector<int>& first, std::vector<int>& second);
		void sort_routes();

		double delta_f(const std::vector<int>& route, const int i, const int j) const;

		std::mt19937 rng;
		std::uniform_real_distribution<double> u {0, 1};
		std::vector<Point> cities;
		std::vector<std::vector<int>> routes;
};

inline Salesman::Salesman(const std::vector<Point>& cities, int population, std::uint_fast32_t seed) : cities{cities}, routes(population), rng{seed}
{
	std::vector<int> route;
	for (int i = 0; i < cities.size(); ++i)
		route[i] = i;

	for (int i = 0; i < population; ++i) {
		std::shuffle(route.begin(), route.end(), rng);
		routes[i] = route;
	}

	sort_routes();
}

inline void Salesman::simulate(int generations)
{
	for (int i = 0; i < generations; ++i)
		evolve();
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

inline void Salesman::evolve()
{
	std::vector<std::vector<int>> best_routes(routes.begin(), routes.begin() + routes.size() / 2);
	std::uniform_int_distribution<int> idx(0, routes.size() - 1);
	for (auto& r : routes) {
		r = crossover(best_routes[idx(rng)], best_routes[idx(rng)]);	// Self-pollination allowed
		mutate(r);
	}
}

inline void Salesman::mutate(std::vector<int>& route)
{
	std::uniform_int_distribution<int> idx(0, route.size() - 1);
	int i {idx(rng)};
	int j;
	do {
		j = idx(rng);
	} while (j == i);

	// Only accept good mutations for now
	if (delta_f(route, i, j) < 0)
		std::swap(route[i], route[j]);
}

inline std::vector<int> Salesman::crossover(std::vector<int>& first, std::vector<int>& second)
{
	assert(first.size() == second.size());
	std::vector<int> child(first.size());

	// Pick first node randomly between parents
	auto first_it = first.begin();
	auto second_it = second.begin();
	child[0] = u(rng) < 0.5 ? *(first_it++) : *(second_it++);
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

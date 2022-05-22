#ifndef ROUTE_HPP
#define ROUTE_HPP
#include <algorithm>
#include <cassert>
#include <mpi.h>
#include <random>
#include <vector>
#include "point.hpp"

class Route {
    public:
        Route(std::vector<Point>* cities);
        Route(const Route& first, const Route& second);
        int send(int dest, int tag=0);
        int recv(int source, MPI_Status* status, int tag=0);

        template <typename RNG>
        void shuffle(RNG rng);
        template <typename RNG>
		void mutate(RNG rng);

        double delta_length(const int i, const int j) const;
        double calculate_length();
        double get_length() const {return length;}

        friend void swap(Route& first, Route& second);
    private:
        void swap_indices(const int i, const int j);

        std::vector<Point>* cities;
        std::vector<int> route;
        double length;
};

inline Route::Route(std::vector<Point>* cities) : cities{cities}, route(cities->size())
{
    for (int i = 0; i < cities->size(); ++i)
        route[i] = i;
    calculate_length();
}

inline Route::Route(const Route& first, const Route& second) : cities {first.cities}, route(cities->size())
{
	assert(first.cities == second.cities);

	// First node is always first city
	auto first_it = first.route.begin() + 1;
	auto second_it = second.route.begin() + 1;
	route[0] = 0;
	for (auto it = route.begin() + 1; it < route.end(); ++it) {
		// Advance iterators until we find elements not in child yet
		while (std::find(route.begin(), it, *first_it) < it)
			++first_it;
		while (std::find(route.begin(), it, *second_it) < it)
			++second_it;
		
		// Pick next node as the one with the shorter distance to previous node
		*it = (cities->at(*first_it) - cities->at(*(it - 1))).norm() < (cities->at(*second_it) - cities->at(*(it - 2))).norm() ? *first_it : *second_it;
	}

    calculate_length();
}

inline int Route::send(int dest, int tag) {
    MPI_Send(route.data(), route.size(), MPI_INT, dest, tag, MPI_COMM_WORLD);
    return MPI_Send(&length, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
}

inline int Route::recv(int source, MPI_Status* status, int tag)
{
    MPI_Recv(route.data(), route.size(), MPI_INT, source, tag, MPI_COMM_WORLD, status);
    return MPI_Recv(&length, 1, MPI_INT, source, tag, MPI_COMM_WORLD, status);
}

template <typename RNG>
inline void Route::shuffle(RNG rng)
{
    std::shuffle(route.begin() + 1, route.end(), rng);
    calculate_length();
}

template <typename RNG>
void Route::mutate(RNG rng)
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
	if (delta_length(i, j) < 0) {
	 	swap_indices(i, j);
        calculate_length();
    }
}

inline void Route::swap_indices(const int i, const int j)
{
    std::swap(route[i], route[j]);
    calculate_length();
}

inline double Route::calculate_length()
{
	length = 0.0;
	for (int i = 0; i < route.size() - 1; ++i)
		length += (cities->at(route[i]) - cities->at(route[i+1])).norm();
	length += (cities->at(route[route.size() - 1]) - cities->at(route[0])).norm();
	return length;
}

// Extremely cursed
inline double Route::delta_length(const int i, const int j) const
{
	int prev_i = i == 0 ? cities->size() - 1 : i - 1;
	int next_i = i == cities->size() - 1 ? 0 : i + 1;

	int prev_j = j == 0 ? cities->size() - 1 : j - 1;
	int next_j = j == cities->size() - 1 ? 0 : j + 1;

	if (i == prev_j)
		return (cities->at(route[i]) - cities->at(route[next_j])).norm() + (cities->at(route[j]) - cities->at(route[prev_i])).norm() -
                (cities->at(route[i]) - cities->at(route[prev_i])).norm() - (cities->at(route[j]) - cities->at(route[next_j])).norm();
	else if (i == next_j)
		return (cities->at(route[i]) - cities->at(route[prev_j])).norm() + (cities->at(route[j]) - cities->at(route[next_i])).norm() -
            (cities->at(route[i]) - cities->at(route[next_i])).norm() - (cities->at(route[j]) - cities->at(route[prev_j])).norm();
	else 
		return (cities->at(route[i]) - cities->at(route[prev_j])).norm() + (cities->at(route[i]) - cities->at(route[next_j])).norm() +
            (cities->at(route[j]) - cities->at(route[prev_i])).norm() + (cities->at(route[j]) - cities->at(route[next_i])).norm() -
            (cities->at(route[i]) - cities->at(route[prev_i])).norm() - (cities->at(route[i]) - cities->at(route[next_i])).norm() -
            (cities->at(route[j]) - cities->at(route[prev_j])).norm() - (cities->at(route[j]) - cities->at(route[next_j])).norm();
}

inline void swap(Route& first, Route& second)
{
    std::swap(first.cities, second.cities);
    std::swap(first.route, second.route);
    std::swap(first.length, second.length);
}

#endif
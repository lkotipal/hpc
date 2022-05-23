#ifndef ROUTE_HPP
#define ROUTE_HPP
#include <algorithm>
#include <cassert>
#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>
#include "point.hpp"

class Route {
    public:
        // Generate ordered route from vector of cities
        Route(std::vector<Point>* cities);

        // Cross-over function
        Route(const Route& first, const Route& second);

        // Communication of route and its length between processes
        int send(int dest);
        int recv(int source, MPI_Status* status);

        // Shuffle order of indices [1, route.size())
        template <typename RNG>
        void shuffle(RNG rng);

        // Attempt to mutate by swapping two random indices
        // This is accepted if it shortens the route
        template <typename RNG>
		void mutate(RNG rng);

        // Calculate difference in route length from swapping two indices
        double delta_length(const int i, const int j) const;

        // Return cached length
        double get_length() const {return length;}

        // Specialized swap function for efficient sorting
        friend void swap(Route& first, Route& second);
        friend std::ostream& operator<<(std::ostream& os, const Route& r);
    private:
        // Swap two indices of the route
        void swap_indices(const int i, const int j);

        // Calculate route length and cache it
        void calculate_length();

        // Pointer to vector of cities for calculating length
        // Memory management handled elsewhere
        std::vector<Point>* cities;

        // Route as a vector of integers, corresponding to positions of vertices
        std::vector<int> route;

        // Cached length of the route
        double length;
};

template <typename RNG>
void Route::shuffle(RNG rng)
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

#endif
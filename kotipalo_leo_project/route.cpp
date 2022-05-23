#include "route.hpp"

Route::Route(std::vector<Point>* cities) : cities{cities}, route(cities->size())
{
    for (int i = 0; i < cities->size(); ++i)
        route[i] = i;
    calculate_length();
}

Route::Route(const Route& first, const Route& second) : cities {first.cities}, route(cities->size())
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

int Route::send(int dest) {
    MPI_Send(&length, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    return MPI_Send(route.data(), route.size(), MPI_INT, dest, 1, MPI_COMM_WORLD);
}

int Route::recv(int source, MPI_Status* status)
{
    MPI_Recv(&length, 1, MPI_INT, source, 0, MPI_COMM_WORLD, status);
    return MPI_Recv(route.data(), route.size(), MPI_INT, source, 1, MPI_COMM_WORLD, status);
}

void Route::swap_indices(const int i, const int j)
{
    std::swap(route[i], route[j]);
    calculate_length();
}

void Route::calculate_length()
{
	length = 0.0;
	for (int i = 0; i < route.size() - 1; ++i)
		length += (cities->at(route[i]) - cities->at(route[i+1])).norm();
	length += (cities->at(route[route.size() - 1]) - cities->at(route[0])).norm();
}

// Extremely cursed
double Route::delta_length(const int i, const int j) const
{
    // Trivial routes
    if (route.size() < 4)
        return 0.0;

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

void swap(Route& first, Route& second)
{
    std::swap(first.cities, second.cities);
    std::swap(first.route, second.route);   // Vector swap is O(1)
    std::swap(first.length, second.length);
}

std::ostream& operator<<(std::ostream& os, const Route& r)
{
    for (int i : r.route)
        os << r.cities->at(i) << std::endl;
    return os;
}
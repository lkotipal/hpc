#include "salesman.hpp"
#include <algorithm>
#include <cmath>
#include <mpi.h>

int Salesman::simulate(int generations)
{
    // Shuffle routes before each simulation
	shuffle_routes();
	double len = routes[0].get_length();
	int i {0};
	int last_improved {0};
	while (i - last_improved < generations) {
		++i;
		evolve();

        // Check if any process has improved
		double new_len = best_route().get_length();
		if (new_len < len) {
			len = new_len;
			last_improved = i;
		}
		MPI_Allreduce(&last_improved, &last_improved, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	}
	return last_improved;
}

void Salesman::communicate()
{
	int id, ntasks;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

    // No communication needed
    if (ntasks < 2)
        return;

	int left = id ? id - 1 : ntasks - 1;
	int right = id < ntasks - 1 ? id + 1 : 0;
	
	for (int i = 0; i < routes.size() / 4; ++i) {
        routes[i].send(left);
        routes[routes.size() / 2 + i].recv(right, &status);
        if (ntasks > 2) {
            routes[i].send(right);
            routes[3 * routes.size() / 4 + 1].recv(left, &status);
        }
	}
	sort_routes();
}

void Salesman::evolve()
{
	std::vector<Route> best_routes(routes.begin(), routes.begin() + routes.size() / 2);
	std::uniform_int_distribution<int> idx(0, best_routes.size() - 1);
	for (auto i = routes.begin() + std::max(1, static_cast<int>(routes.size() / 8)); i < routes.end(); ++i) {
		*i = Route(best_routes[idx(rng)], best_routes[idx(rng)]);	// Self-pollination allowed
        i->mutate(rng);
    }
	sort_routes();
	communicate();
}

void Salesman::shuffle_routes()
{
	for (int i = 0; i < routes.size(); ++i) {
		routes[i].shuffle(rng);
	}
	sort_routes();
}

void Salesman::sort_routes()
{
	std::sort(routes.begin(), routes.end(), [](const Route& a, const Route& b) -> bool {
		return a.get_length() < b.get_length();
	});
}
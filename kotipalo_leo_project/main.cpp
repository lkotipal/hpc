#include <cmath>
#include <fstream>
#include <iostream>
#include <numbers>
#include "point.hpp"
#include "salesman.hpp"

int main(int argc, char *argv[]) 
{
	std::uint_fast32_t seed = 1;
	if (argc > 1) {
		try {
			seed = std::stoi(argv[1]);
		} catch (std::invalid_argument e) {
			std::clog << "Invalid seed given." << std::endl;
		}
	} else {
		std::clog << "No seed given." << std::endl;
	}
	std::clog << "Using seed " << seed << std::endl;

	std::mt19937 rng {seed};
	std::uniform_real_distribution<double> u {0, 1};
	std::uniform_real_distribution<double> theta {0, 2 * std::numbers::pi};
	constexpr int population {10};
	constexpr int generations {1'000};

	std::vector<Point> square_points;
	for (int i = 0; i < 20; ++i)
		square_points.push_back(Point(std::array<double, 2>{u(rng), u(rng)}));

	std::vector<Point> circle_points;
	for(int i = 0; i < 20; ++i)
		circle_points.push_back(Point(1, theta(rng)));

	std::vector<Point> cities;
	std::ifstream f {"20cities.dat"};
	while (!f.eof()) {
		double x, y;
		std::string name;
		f >> x >> y >> name;
		cities.push_back(Point(std::array<double, 2>{x, y}));
	}
	f.close();

	std::array<std::vector<Point>, 3> point_vecs {square_points, circle_points, cities}; 
	std::array<std::string, 3> names {"square", "circle", "cities"};
	
	for (int i = 0; i < 3; ++i) {
		auto points = point_vecs[i];
		Salesman sm {points, population, seed};

		sm.simulate(generations);
		auto best_route = sm.best_route();
		double l = sm.f(best_route);

		std::cout << "Best route with length " << l << ":" << std::endl;
		for (int j : best_route) {
			Point p = points[j];
			std::cout << std::fixed << p[0] << "\t" << p[1] << std::endl;
		}
	}

	return 0;
}
#include <array>
#include <chrono>
#include <iostream>
#include <random>

template <size_t n>
double calculate_unoptimized(std::array<std::array<double, n>, n>& a, const std::array<std::array<double, n>, n>& b, const std::array<double, n>& c)
{
	auto start = std::chrono::steady_clock::now();
	for (int j = 0; j < n; ++j)
		for (int i = 0; i < n; ++i)
			a[i][j] = b[i][j] / c[i];
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double calculate_optimized(std::array<std::array<double, n>, n>& a, const std::array<std::array<double, n>, n>& b, const std::array<double, n>& c)
{
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			a[i][j] = b[i][j] / c[i];
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

int main(int argc, char *argv[])
{
	constexpr int n = 3'000;
	std::array<std::array<double, n>, n> a{};
	std::array<std::array<double, n>, n> b{};
	std::array<double, n> c{};

	// Populate arrays
	std::mt19937 rng; // Default seed for consistency
	std::uniform_real_distribution<> dis;
	for (auto& i : b)
		for (double& d : i)
			d = dis(rng);
	for (double& d : c)
		d = dis(rng);
	
	double t = calculate_unoptimized(a, b, c);
	std::clog << a[n-1][n-1] << std::endl;
	a.fill({});
	double t_opt = calculate_optimized(a, b, c);
	std::clog << a[n-1][n-1] << std::endl;

	std::cout << std::fixed << t << "\t" << t_opt << std::endl;
}
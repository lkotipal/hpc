#include <array>
#include <chrono>
#include <iostream>
#include <random>

template <size_t n>
double calculate_unoptimized(std::array<double, n>& a, const std::array<double, n>& b)
{
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < n - 1; ++i)
		if (i < 499)	// Off by one compared to Fortran ver
			a[i] = 4.0 * b[i] + b[i+1];
		else
			a[i] = 4.0 * b[i+1] + b[i];
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double calculate_optimized(std::array<double, n>& a, const std::array<double, n>& b)
{
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < 499; ++i)
		a[i] = 4.0 * b[i] + b[i+1];
	for (int i = 499; i < n - 1; ++i)
		a[i] = 4.0 * b[i+1] + b[i];
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

int main(int argc, char *argv[])
{
	constexpr int n = 10'000'000;
	std::array<double, n> a{};
	std::array<double, n> b{};

	// Populate arrays
	std::mt19937 rng; // Default seed for consistency
	std::uniform_real_distribution<> dis;
	for (double& d : b)
		d = dis(rng);
	
	double t = calculate_unoptimized(a, b);
	std::clog << a[n-2] << std::endl;
	a.fill(0.0);
	double t_opt = calculate_optimized(a, b);
	std::clog << a[n-2] << std::endl;

	std::cout << std::fixed << t << "\t" << t_opt << std::endl;
}
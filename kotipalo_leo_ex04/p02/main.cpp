#include <chrono>
#include <iostream>
#include <random>
#include "matrix.hpp"

template <int n>
double multiply(Matrix<n>& a, Matrix<n>& b, bool transpose_a, bool transpose_b)
{
	auto start = std::clock();
	if (transpose_a)
		a.transpose();
	if (transpose_b)
		b.transpose();
	Matrix<n> c = a * b;
	if (transpose_a)
		a.transpose();
	if (transpose_b)
		b.transpose();
	auto end = std::clock();
	return static_cast<double>(end - start) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[])
{
	constexpr int n = 1'000;
	Matrix<n> a{};
	Matrix<n> b{};

	// Populate arrays
	std::mt19937 rng; // Default seed for consistency
	std::uniform_real_distribution<> dis;
	a.randomize(rng, dis);
	b.randomize(rng, dis);

	// A * B
	double t1 = multiply(a, b, false, false);

	// A^T * B
	double t2 = multiply(a, b, true, false);

	// A * B^T
	double t3 = multiply(a, b, false, true);

	std::cout << std::fixed << t1 << "\t" << t2 << "\t" << t3 << std::endl;
}
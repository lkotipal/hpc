#include <cmath>
#include <chrono>
#include <iostream>

template<typename T>
T f(int n)
{
	T sum = 0;
	for (int k = 0; k <= n; ++k)
		sum += std::exp(std::sin((T) k / 100'000));
	return sum;
}

int main()
{
	// Check floating point sizes
	static_assert(sizeof(float) == 32/8);
	static_assert(sizeof(double) == 64/8);
	static_assert(sizeof(long double) == 128/8);

	int n = 100'000'000;

	auto start = std::chrono::steady_clock::now();
	float sum_float = f<float>(n);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	std::cout << "Using 32-bit floats " << sum_float << " calculated in " << seconds.count() << "s" << std::endl;

	start = std::chrono::steady_clock::now();
	double sum_double = f<double>(n);
	end = std::chrono::steady_clock::now();
	seconds = end - start;
	std::cout << "Using 64-bit floats " << sum_double << " calculated in " << seconds.count() << "s" << std::endl;

	start = std::chrono::steady_clock::now();
	long double sum_long_double = f<long double>(n);
	end = std::chrono::steady_clock::now();
	seconds = end - start;
	std::cout << "Using 128-bit floats " << sum_long_double << " calculated in " << seconds.count() << "s" << std::endl;

	return 0;
}

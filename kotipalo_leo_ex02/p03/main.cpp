#include <cmath>
#include <chrono>
#include <iostream>

template<typename T>
T f(int n)
{
	T sum = 0;
	for (int k = 0; k <= n; ++k)
		sum += std::exp(std::sin(static_cast<T>(k) / 100'000));
	return sum;
}

template<typename T>
void test()
{
	auto start = std::chrono::steady_clock::now();
	float sum_float = f<T>(100'000'000);
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> seconds = end - start;
	std::cout << "Using " << sizeof(T) * 8 << "-bit floats " << sum_float << " calculated in " << seconds.count() << "s" << std::endl;
}

int main()
{
	// Check floating point sizes
	static_assert(sizeof(float) == 32/8);
	static_assert(sizeof(double) == 64/8);
	static_assert(sizeof(long double) == 128/8);

	test<float>();
	test<double>();
	test<long double>();

	return 0;
}

#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <stdio.h>

template <size_t n>
double write_formatted(std::array<float, n>& a)
{
	FILE *fu = fopen("temp.txt", "w");
	auto start = std::clock();
	for (float f : a)
		fprintf(fu, "%g\n", f);
	auto end = std::clock();
	fclose(fu);
	return static_cast<double>(end - start) / CLOCKS_PER_SEC;
}

template <size_t n>
double write_unformatted(std::array<float, n>& a)
{
	FILE *fu = fopen("temp.bin", "w");
	auto start = std::clock();
	for (float f : a)
		fwrite(&f, sizeof(f), 1, fu);
	auto end = std::clock();
	fclose(fu);
	return static_cast<double>(end - start) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[])
{
	constexpr int n = 10'000'000;
	std::array<float, n> a;

	// Populate arrays
	std::mt19937 rng; // Default seed for consistency
	std::uniform_real_distribution<float> dis{};
	for (float& f : a)
		f = dis(rng);
	
	for (int i = 0; i < 1'000; ++i)
		std::cout << std::fixed << write_formatted(a) << "\t" << write_unformatted(a) << std::endl;

	//std::cout << t1 << "\t" << t2 << "\t" << t3 << std::endl;
}
#include <array>
#include <chrono>
#include <iostream>
#include <random>

template <size_t n>
double unroll_1(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; ++i)
	{
		prod *= a[i];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double unroll_2(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; i += 2)
	{
		prod *= a[i];
		prod *= a[i+1];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double unroll_4(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; i += 4)
	{
		prod *= a[i];
		prod *= a[i+1];
		prod *= a[i+2];
		prod *= a[i+3];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double unroll_8(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; i += 8)
	{
		prod *= a[i];
		prod *= a[i+1];
		prod *= a[i+2];
		prod *= a[i+3];
		prod *= a[i+4];
		prod *= a[i+5];
		prod *= a[i+6];
		prod *= a[i+7];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double unroll_16(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; i += 16)
	{
		prod *= a[i];
		prod *= a[i+1];
		prod *= a[i+2];
		prod *= a[i+3];
		prod *= a[i+4];
		prod *= a[i+5];
		prod *= a[i+6];
		prod *= a[i+7];
		prod *= a[i+8];
		prod *= a[i+9];
		prod *= a[i+10];
		prod *= a[i+11];
		prod *= a[i+12];
		prod *= a[i+13];
		prod *= a[i+14];
		prod *= a[i+15];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

template <size_t n>
double unroll_32(std::array<double, n>& a)
{
	auto start = std::chrono::steady_clock::now();

	double prod = 1.0;
	for (int i = 0; i < n; i += 32)
	{
		prod *= a[i];
		prod *= a[i+1];
		prod *= a[i+2];
		prod *= a[i+3];
		prod *= a[i+4];
		prod *= a[i+5];
		prod *= a[i+6];
		prod *= a[i+7];
		prod *= a[i+8];
		prod *= a[i+9];
		prod *= a[i+10];
		prod *= a[i+11];
		prod *= a[i+12];
		prod *= a[i+13];
		prod *= a[i+14];
		prod *= a[i+15];
		prod *= a[i+16];
		prod *= a[i+17];
		prod *= a[i+18];
		prod *= a[i+19];
		prod *= a[i+20];
		prod *= a[i+21];
		prod *= a[i+22];
		prod *= a[i+23];
		prod *= a[i+24];
		prod *= a[i+25];
		prod *= a[i+26];
		prod *= a[i+27];
		prod *= a[i+28];
		prod *= a[i+29];
		prod *= a[i+30];
		prod *= a[i+31];
	}

	auto end = std::chrono::steady_clock::now();
	std::clog << prod << std::endl;

	std::chrono::duration<double> seconds = end - start;
	return seconds.count();
}

int main(int argc, char *argv[])
{
	constexpr int n = 0b1'0000'0000'0000'0000'0000'0000;
	std::array<double, n> a;

	// Populate arrays
	std::mt19937 rng; // Default seed for consistency
	std::uniform_real_distribution<> dis(0.9, 1.1);
	for (double& d : a)
		d = dis(rng);
	
	double t1 = unroll_1(a);
	double t2 = unroll_2(a);
	double t3 = unroll_4(a);
	double t4 = unroll_8(a);
	double t5 = unroll_16(a);
	double t6 = unroll_32(a);

	std::cout << std::fixed <<  t1 << "\t" << t2 << "\t" << t3 << "\t" << t4 << "\t" << t5 << "\t" << t6 << std::endl;

	//std::cout << t1 << "\t" << t2 << "\t" << t3 << std::endl;
}
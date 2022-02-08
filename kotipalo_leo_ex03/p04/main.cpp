#include <cmath>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>

void write_file(int n)
{
	std::ofstream out_file{"out_04.txt"};
	for (int k = 0; k <= n; ++k)
		if (out_file.is_open())
			out_file << k << std::exp(std::sin(static_cast<double>(k) / 100'000)) << std::endl;
	return;
}

double read_file()
{
	double sum = 0;
	int k = 0;
	double a_k = 0;
	std::ifstream in_file("out_04.txt");
	while (in_file >> k >> a_k){
		sum += a_k;
	}
	return sum;
}

int main()
{
	auto start_wall = std::chrono::system_clock::now();
	auto start_cpu = std::clock();

	write_file(1'000'000);
	double sum = read_file();

	auto end_cpu = std::clock();
	auto end_wall = std::chrono::system_clock::now();
	double seconds_cpu = static_cast<double>(end_cpu - start_cpu) / CLOCKS_PER_SEC;
	std::chrono::duration<double> seconds_wall = end_wall - start_wall;
	std::cout << sum << " calculated in " << seconds_cpu << "s CPU time, " << seconds_wall.count() << "s walltime." << std::endl;
	return 0;
}

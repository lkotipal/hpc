#ifndef REJECTION_SAMPLER_HPP
#define REJECTION_SAMPLER_HPP
#include <random>
#include <mpi.h>

class Rejection_sampler {
	public:
		Rejection_sampler(const std::uint_fast32_t seed) : rng{seed} {};
		static constexpr double f(const double x);
		virtual double integrate(const int n);
		static constexpr double x_min = 0.0;
		static constexpr double x_max = 1.0;
		static constexpr double y_min = 0.0;
		static constexpr double y_max = 1.0;
	private:
		bool generate();
		std::mt19937 rng;
		std::uniform_real_distribution<double> x{x_min, x_max};
		std::uniform_real_distribution<double> y{y_min, y_max};
};

inline constexpr double Rejection_sampler::f(const double x)
{
	return std::sqrt(1 - x*x);
}

inline bool Rejection_sampler::generate()
{
	return f(x(rng)) > y(rng);
}

inline double Rejection_sampler::integrate(const int n)
{
	int hits = 0;
	for (int i = 0; i < n; ++i)
		if (generate())
			++hits;
	return static_cast<double>(hits) / n;
}

#endif
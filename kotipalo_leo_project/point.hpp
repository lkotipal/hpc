#ifndef POINT_HPP
#define POINT_HPP
#include <array>
#include <cmath>
#include <random>

class Point {
	public:
		Point(const std::array<double, 2>& x) : x{x} {}
		Point(const double r, const double theta) : Point(std::array<double, 2> {r * std::cos(theta), r * std::sin(theta)}) {}
		double norm() const {return std::sqrt(x[0] * x[0] + x[1] * x[1]);}
		Point operator-() const {return Point(std::array<double, 2> {-x[0], -x[1]});}
		double operator[](int idx) const {return x[idx];}
	private:
		std::array<double, 2> x;
		inline static std::normal_distribution<double> dis;
};

inline Point operator+(const Point& p1, const Point& p2)
{
	return Point(std::array<double, 2> {p1[0] + p2[0], p1[1] + p2[1]});
}

inline Point operator-(const Point& p1, const Point& p2)
{
	return p1 + -p2;
}

inline Point operator*(const double r, const Point& p)
{
	return Point(std::array<double, 2> {r * p[0], r * p[1]});
}

inline Point operator*(const Point& p, const double r)
{
	return r * p;
}

inline Point operator/(const Point& p, const double r)
{
	return p * (1.0 / r);
}

#endif
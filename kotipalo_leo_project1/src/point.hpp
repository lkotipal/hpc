#ifndef POINT_HPP
#define POINT_HPP
#include <array>
#include <cmath>
#include <iostream>

class Point {
	public:
		Point(const std::array<double, 2>& x) : x{x} {}
		Point(const double r, const double theta) : Point(std::array<double, 2> {r * std::cos(theta), r * std::sin(theta)}) {}
		double norm() const {return std::sqrt(x[0] * x[0] + x[1] * x[1]);}
		Point operator-() const {return Point(std::array<double, 2> {-x[0], -x[1]});}
		double operator[](int idx) const {return x[idx];}
	private:
		std::array<double, 2> x;
};

// Addition of vectors
Point operator+(const Point& p1, const Point& p2);

// Subtraction of vectors
Point operator-(const Point& p1, const Point& p2);

// Multiplying a vector with a real
Point operator*(const double r, const Point& p);
Point operator*(const Point& p, const double r);

// Dividing a vector with a real
Point operator/(const Point& p, const double r);

// ostream output
std::ostream& operator<<(std::ostream& os, const Point& p);

#endif
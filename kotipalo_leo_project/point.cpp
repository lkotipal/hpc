#include "point.hpp"

Point operator+(const Point& p1, const Point& p2)
{
	return Point(std::array<double, 2> {p1[0] + p2[0], p1[1] + p2[1]});
}

Point operator-(const Point& p1, const Point& p2)
{
	return p1 + -p2;
}

Point operator*(const double r, const Point& p)
{
	return Point(std::array<double, 2> {r * p[0], r * p[1]});
}

Point operator*(const Point& p, const double r)
{
	return r * p;
}

Point operator/(const Point& p, const double r)
{
	return p * (1.0 / r);
}

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << p[0] << "\t" << p[1];
    return os;
}
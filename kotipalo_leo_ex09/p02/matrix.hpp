#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <cassert>
#include <iostream>
#include <memory>

class Matrix {
    public:
        Matrix(int n);
        double* data() const;
        double& operator()(int y, int x);
        double operator()(int y, int x) const;
        const int n;
    private:
        std::unique_ptr<double> arr;
};

inline Matrix::Matrix(int n) : n{n}, arr{new double[n*n]} 
{
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            this->operator()(i, j) = 0;
}

inline double* Matrix::data() const
{
    return arr.get();
}

inline double& Matrix::operator()(int y, int x)
{
    assert (y >= 0 && y < n);
    assert (x >= 0 && x < n);
    return data()[n*y + x];
}

inline double Matrix::operator()(int y, int x) const
{
    return data()[n*y + x];
}

inline std::ostream& operator<<(std::ostream& out, const Matrix& m)
{
		for (int i = 0; i < m.n; ++i) {
			for (int j = 0; j < m.n; ++j)
				out << m(i, j) << " ";
			out << "\n";
		}
        return out;
}

#endif
#ifndef DISTRIBUTED_MATRIX_HPP
#define DISTRIBUTED_MATRIX_HPP
#include "matrix.hpp"

class Distributed_matrix : public Matrix {
    public:
        Distributed_matrix(int n) : Matrix(n+2) {};
        double* data_at(int y, int x) const;
        double& operator()(int y, int x);
        double operator()(int y, int x) const;
};

inline double* Distributed_matrix::data_at(int y, int x) const
{
    assert(y >= -1 && y < n + 1);
    assert(x >= -1 && x < n + 1);
    return Matrix::data() + n * (y + 1) + x + 1;
}

inline double& Distributed_matrix::operator()(int y, int x)
{
    return *data_at(y, x);
}

inline double Distributed_matrix::operator()(int y, int x) const
{
    return *data_at(y, x);
}

#endif
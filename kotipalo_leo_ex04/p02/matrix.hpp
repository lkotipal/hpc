#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <array>

template <int n>
class Matrix {
public:
	Matrix() {}
	double& operator()(int i, int j);
	double operator()(int i, int j) const;
	void transpose();

	template <int m>
	friend Matrix<m> operator*(const Matrix<m> &a, const Matrix<m> &b);

	template<class RNG, class DIS>
	void randomize(RNG rng, DIS dis);
private:
	std::array<std::array<double, n>, n> data{};
	bool transposed{false};
};

template <int n>
double& Matrix<n>::operator()(int i, int j)
{
	return transposed ? data[j][i] : data[i][j];
}

template <int n>
double Matrix<n>::operator()(int i, int j) const
{
	return transposed ? data[j][i] : data[i][j];
}

template <int n>
void Matrix<n>::transpose()
{
	this->transposed = !this->transposed;
}

template <int n>
Matrix<n> operator*(const Matrix<n> &a, const Matrix<n> &b)
{
	Matrix<n> c;
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			for (int k = 0; k < n; ++k)
				c(i, j) += a(i, k) * b(k, j);
	return c;
}

template <int n>
template <class RNG, class DIS>
inline void Matrix<n>::randomize(RNG rng, DIS dis)
{
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
			(*this)(i, j) = dis(rng);
}

#endif
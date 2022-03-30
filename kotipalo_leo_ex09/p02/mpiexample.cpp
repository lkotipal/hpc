#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mpi.h>
#include <vector>
#include "matrix.hpp"
#include "distributed_matrix.hpp"

void scatter_matrix(const Matrix* matrix_send, MPI_Datatype datatype_send, Distributed_matrix* matrix_recv, MPI_Datatype datatype_recv, int root, MPI_Comm communicator)
{
	int tag{0};
	int id;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int dims[2];
	int periods[2];
	int coords[2];
	MPI_Cart_get(communicator, 2, dims, periods, coords);

	int m_p = matrix_recv->n - 2;

	if (id == root) {
		for (int i = 0; i < dims[0]; ++i) {
			for (int j = 0; j < dims[1]; ++j) {
				int coords[2] {i, j};
				int dest;
				MPI_Cart_rank(communicator, coords, &dest);
				MPI_Send(matrix_send->data() + dims[0] * m_p * m_p * j + m_p * i, 1, datatype_send, dest, tag, communicator);
			}
		}
	}

	MPI_Recv(matrix_recv->data_at(0, 0), 1, datatype_recv, root, tag, communicator, &status);
}

void gather_matrix(const Distributed_matrix* matrix_send, MPI_Datatype datatype_send, Matrix* matrix_recv, MPI_Datatype datatype_recv, int root, MPI_Comm communicator)
{
	int tag{1};
	int id;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int dims[2];
	int periods[2];
	int coords[2];
	MPI_Cart_get(communicator, 2, dims, periods, coords);

	int m_p = matrix_send->n - 2;

	MPI_Send(matrix_send->data_at(0, 0), 1, datatype_send, root, tag, communicator);
	if (id == root) {
		for (int i = 0; i < dims[0]; ++i) {
			for (int j = 0; j < dims[1]; ++j) {
				int coords[2] {i, j};
				int src;
				MPI_Cart_rank(communicator, coords, &src);
				MPI_Recv(matrix_recv->data() + dims[0] * m_p * m_p * j + m_p * i, 1, datatype_recv, src, tag, communicator, &status);
			}
		}
	}
}

void communicate_boundaries(const Distributed_matrix* matrix, MPI_Datatype row_type, MPI_Datatype col_type, MPI_Comm communicator)
{
	int tag{1};
	int id;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int my_coords[2];
	MPI_Cart_coords(communicator, id, 2, my_coords);

	int left;
	int left_coords[2] {my_coords[0] - 1, my_coords[1]};
	MPI_Cart_rank(communicator, left_coords, &left);

	int right;
	int right_coords[2] {my_coords[0] + 1, my_coords[1]};
	MPI_Cart_rank(communicator, right_coords, &right);

	int up;
	int up_coords[2] {my_coords[0], my_coords[1] - 1};
	MPI_Cart_rank(communicator, up_coords, &up);

	int down;
	int down_coords[2] {my_coords[0], my_coords[1] + 1};
	MPI_Cart_rank(communicator, down_coords, &down);

	int m_p = matrix->n - 2;

	// Send up, receive down
	MPI_Sendrecv(matrix->data_at(0, 0), 1, row_type, up, tag, matrix->data_at(m_p, 0), 1, row_type, down, tag, communicator, &status);
	// Send down, receive up
	MPI_Sendrecv(matrix->data_at(m_p - 1, 0), 1, row_type, down, tag, matrix->data_at(-1, 0), 1, row_type, up, tag, communicator, &status);

	// Send left, receive right
	MPI_Sendrecv(matrix->data_at(0, 0), 1, col_type, left, tag, matrix->data_at(0, m_p), 1, col_type, right, tag, communicator, &status);
	// Send right, receive left
	MPI_Sendrecv(matrix->data_at(0, m_p - 1), 1, col_type, right, tag, matrix->data_at(0, -1), 1, col_type, left, tag, communicator, &status);

}

int main(int argc,char *argv[])
{
	constexpr int tag{50};
	int id, ntasks, rc;
	MPI_Status status;

	if (MPI_Init(&argc,&argv) != MPI_SUCCESS) {
		std::clog << "MPI initialization failed" << std::endl;
		exit(1);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);

	int m{16};
	if (argc > 1) {
		try {
			m = std::stoi(argv[1]);
			if (m < 0) {
				if (id == 0)
					std::clog << "m must be a positive integer." << std::endl;
			}
		} catch (std::invalid_argument e) {
			if (id == 0)
				std::clog << "m must be a positive integer." << std::endl;
		}
	}
	if (id == 0) {
		std::clog << "Using m = " << m << std::endl;
		std::clog << "Running on " << ntasks << " processes" << std::endl;
	}

	const int p{static_cast<int>(sqrt(ntasks))};
	assert (p == sqrt(ntasks));
	assert (m % p == 0);

	Matrix* all_x = nullptr;
	if (id == 0) {
		all_x = new Matrix(m);
		for (int i = 0; i < m; ++i)
			for (int j = 0; j < m; ++j)
				(*all_x)(i, j) = (i + 1) + (j + 1);
	}

	int m_p = m / p;
	Distributed_matrix* my_x = new Distributed_matrix(m_p);

	MPI_Datatype matrix_type;
	MPI_Type_vector(m_p, m_p, m, MPI_DOUBLE, &matrix_type);
	MPI_Type_commit(&matrix_type);

	MPI_Datatype distributed_matrix_type;
	MPI_Type_vector(m_p, m_p, my_x->n, MPI_DOUBLE, &distributed_matrix_type);
	MPI_Type_commit(&distributed_matrix_type);

	MPI_Comm cartesian_comm;
	int dims[2] {p, p};
	int periodic[2] {true, true};
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, true, &cartesian_comm);

	scatter_matrix(all_x, matrix_type, my_x, distributed_matrix_type, 0, cartesian_comm);

	MPI_Datatype col_type;
	MPI_Type_vector(m_p, 1, my_x->n, MPI_DOUBLE, &col_type);
	MPI_Type_commit(&col_type);

	MPI_Datatype row_type;
	MPI_Type_vector(1, m_p, my_x->n, MPI_DOUBLE, &row_type);
	MPI_Type_commit(&row_type);

	if (id == 0)
		std::cout << "t = 0" << std::endl << *all_x << std::endl;

	Distributed_matrix next_x(m/p);
	for (int t = 0; t <= 50; ++t)
	{
		communicate_boundaries(my_x, row_type, col_type, cartesian_comm);

		for (int i = 0; i < m_p; ++i)
			for (int j = 0; j < m_p; ++j)
				next_x(i, j) = ((*my_x)(i-1, j) + (*my_x)(i, j-1) + (*my_x)(i+1, j) + (*my_x)(i, j+1)) / 8.0 + (*my_x)(i, j);

		for (int i = 0; i < m_p; ++i)
			for (int j = 0; j < m_p; ++j)
				(*my_x)(i, j) = next_x(i, j);

		switch (t) {
		case 10:
		case 50:
			gather_matrix(my_x, distributed_matrix_type, all_x, matrix_type, 0, cartesian_comm);
			if (id == 0)
				std::cout << "t = " << t << std::endl << *all_x << std::endl;
		}
	}

	MPI_Finalize();
	exit(0);
}

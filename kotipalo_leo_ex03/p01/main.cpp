#include <iostream>
#include <string>

constexpr int ackermann(int m, int n)
{
	if (m == 0)
		return n + 1;
	else if (n == 0)
		return ackermann(m - 1, 1);
	else
		return ackermann(m - 1, ackermann(m, n - 1));
}

int main(int argc, char *argv[])
{
	int m {0};
	int n {0};
	if (argc > 2) {
		try {
			m = std::stoi(argv[1]);
			n = std::stoi(argv[2]);
		} catch (std::invalid_argument e) {
			std::clog << "m, n must be integers." << std::endl;
		}
	}
	std::clog << "(m, n) = (" << m << ", " << n << ")" << std::endl;
	std::cout << ackermann(m, n) << std::endl;
	return 0;
}
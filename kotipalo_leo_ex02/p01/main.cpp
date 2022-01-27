#include <iostream>

int main()
{
	int a[10];
	for (int i = 0;; ++i) {
		std::cout << "a[" << i << "] = "  << a[i] << std::endl;
	}
	return 0;
}
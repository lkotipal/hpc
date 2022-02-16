//Hello Antti, // //Please find the example code from below. // //Cheers, //Xin #include <iostream>

#include <ctime>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  int n2 = 0;
	if (argc > 1) {
		try {
			n2 = std::stoi(argv[1]);
		} catch (std::invalid_argument e) {
			std::clog << "n must be integer." << std::endl;
		}
	}

  //int n2 = 2560;
  int n3 = 20000000;
  int *d = new int [n2];
  int *e = new int [n2];
  int *f = new int [n2];
  clock_t v1, v2;
  /*---------Initialize vectors---------*/
  v1 = std::clock();
  for (int i=0; i<n2; i++) {
    d[i] = 5;
    e[i] = 5 + 3;
    f[i] = 0;
  }
  v2 = std::clock();
  //std::cout << "Initialize vector: \t" << static_cast<double>(v2-v1)/CLOCKS_PER_SEC << std::endl;
  clock_t v3, v4;
  /*---------f = d * e (repeat)---------*/
  v3 = std::clock();
  for (int i=0; i<n3; i++) {
    for (int j=0; j<n2; j++) {
      f[j] = d[j] * e[j];
    }
  }
  v4 = std::clock();
  //std::cout << "de(repeat): \t" << static_cast<double>(v4-v3)/CLOCKS_PER_SEC << std::endl;
  std::cout << std::fixed << static_cast<double>(v2-v1)/CLOCKS_PER_SEC << "\t" << static_cast<double>(v4-v3)/CLOCKS_PER_SEC << std::endl;
  delete[] d;
  delete[] e;
  delete[] f;
  return 0;
}

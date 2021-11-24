#include "Eigen/Eigen"
#include <iostream>
#include <time.h>

int main()
{
	srand(time(NULL));
	Eigen::VectorXd x(3);
	x.setRandom();
	std::cout << x << std::endl;
	system("pause");
	return 0;
}
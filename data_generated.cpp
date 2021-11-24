#include <iostream>
#include <random>
#include <time.h>
#include "Eigen/Eigen"
#include "MVN.h"

int main()
{
	FILE *fp = fopen("data.txt", "w");

	srand(time(NULL));
	int K, m;
	double C_l, C_h, R_l, R_h;
	int N_l, N_h;

	
	printf("number of cluster, K = "); scanf("%d", &K);
	
	//printf("number of dimension (including label), m = "); scanf("%d", &m);
	m = 2;

	printf("Centroid Range, C_l = "); scanf("%lf", &C_l);
	printf("Centroid Range, C_h = "); scanf("%lf", &C_h);
	printf("Radius Range, R_l = "); scanf("%lf", &R_l);
	printf("Radius Range, R_h = "); scanf("%lf", &R_h);
	printf("Cluster Point Number Range, N_l = "); scanf("%d", &N_l);
	printf("Cluster Point Number Range, N_h = "); scanf("%d", &N_h);

	double threshold = (C_l + C_h) / 2;
	Eigen::MatrixXf Centroid_Point = Eigen::MatrixXf::Random(K, 2) + Eigen::MatrixXf::Ones(K, 2);
	Centroid_Point = Centroid_Point * (C_h - C_l) / 2 + Eigen::MatrixXf::Constant(Centroid_Point.rows(), Centroid_Point.cols(), C_l);
	
	//std::cout << Centroid_Point << std::endl;
	
	Eigen::VectorXd Radius = Eigen::VectorXd::Random(K) + Eigen::VectorXd::Ones(K);
	Radius = Radius * (R_h - R_l) / 2 + Eigen::VectorXd::Constant(Radius.rows(), Radius.cols(), R_l);

	Eigen::VectorXi Label = Eigen::VectorXi::Zero(K);

	Eigen::VectorXd Numberd = Eigen::VectorXd::Random(K) + Eigen::VectorXd::Ones(K);
	Numberd = Numberd * (N_h - N_l) / 2 + Eigen::VectorXd::Constant(Numberd.rows(), Numberd.cols(), N_l);
	
	Eigen::VectorXi Numberi(K);
	Numberi = Numberd.cast<int>();

	int n = Numberi.sum();

	for (int i = 0; i < K; i++)
	{
		if (Centroid_Point(i, 0) < threshold - Radius[i]) Label[i] = 1;
		else if (Centroid_Point(i, 0) > threshold + Radius[i]) Label[i] = -1;
		else n = n - Numberi[i];
	}

	std::default_random_engine gen;
	Eigen::MatrixXd cov(2, 2);
	cov << 1, 0.5, 0.5, 1;

	fprintf(fp, "%d %d\n", n, m);
	for (int i = 0; i < K; i++)
	if (Label[i] != 0)
	{
		Eigen::VectorXd Current_Centroid_Point(2);
		Current_Centroid_Point << Centroid_Point(i, 0), Centroid_Point(i, 1);
		MVN mvn(Current_Centroid_Point, cov);
		int j = 0;
		for (j = 0; j < Numberi[i]; j++)
		{
			Eigen::VectorXd point;
			point = mvn.sample(200);
			fprintf(fp, "%lf %lf %d\n", point[0], point[1], Label[i]);
		}
	}
	std::cout << "generated successfully" << std::endl;
	system("pause");
	return 0;
}
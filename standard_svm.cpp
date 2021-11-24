#include "my_svm.h"
#include <iostream>
#include <time.h>

int main()
{
	SVM_Solver svm_solver;
	int n, m;
	clock_t start, finish;
	FILE *fp = fopen("train.txt", "r");

	fscanf(fp, "%d %d", &n, &m);
	svm_solver.prob.l = n;
	svm_solver.prob.x = new svm_node*[n];
	svm_solver.prob.y = new double[n];

	start = clock();
	for (int i = 0; i < n; i++)
	{
		svm_solver.prob.x[i] = new svm_node[m + 1];
		for (int j = 0; j < m; j++)
		{
			svm_solver.prob.x[i][j].index = j + 1;
			fscanf(fp, "%lf", &svm_solver.prob.x[i][j].value);
		}
		svm_solver.prob.x[i][m].index = -1;
		fscanf(fp, "%lf", &svm_solver.prob.y[i]);
	}
	svm_solver.param.svm_type = C_SVC;
	svm_solver.param.kernel_type = LINEAR;
	svm_solver.param.degree = 3;
	svm_solver.param.gamma = 1.0 / m;	// 1/num_features
	svm_solver.param.coef0 = 0;
	svm_solver.param.nu = 0.5;
	svm_solver.param.cache_size = 100;
	svm_solver.param.C = 1;
	svm_solver.param.eps = 1e-3;
	svm_solver.param.p = 0.1;
	svm_solver.param.shrinking = 1;
	svm_solver.param.probability = 0;
	svm_solver.param.nr_weight = 0;
	svm_solver.param.weight_label = NULL;
	svm_solver.param.weight = NULL;

	svm_solver.model = svm_train(&svm_solver.prob, &svm_solver.param);

	svm_save_model("standard_train.model", svm_solver.model);
	
	finish = clock();
	std::cout << "standard train successfully" << std::endl;
	std::cout << "time = " << finish - start << std::endl;
	system("pause");
	return 0;
}
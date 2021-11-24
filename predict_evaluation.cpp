#include "my_svm.h"
#include <iostream>
#include <string>

int main()
{
	SVM_Solver svm_solver[2];
	svm_solver[0].model = svm_load_model("standard_train.model");
	svm_solver[1].model = svm_load_model("CF_tree.model");
	FILE *fp = fopen("test.txt", "r");
	int n, m;
	fscanf(fp, "%d %d", &n, &m);
	int correct[2] = {0, 0};
	for (int i = 0; i < n; i++)
	{
		svm_node *node = new svm_node[m + 1];
		for (int j = 0; j < m; j++)
		{
			double value;
			fscanf(fp, "%lf", &value);
			node[j].index = j + 1;
			node[j].value = value;
		}
		node[m].index = -1;
		double correct_type;
		fscanf(fp, "%lf", &correct_type);
		double pred[2];
		for (int j = 0; j < 2; j++)
		{
			pred[j] = svm_predict(svm_solver[j].model, node);
			if (correct_type == pred[j])
				correct[j] = correct[j] + 1;
			//std::cout << i << " " << pred << " " << correct_type << std::endl;
		}
	}
	std::cout << "standard_train.model" << std::endl;
	std::cout << "    accuracy : " << 1.0 * correct[0] / n << std::endl;
	std::cout << "- - -" << std::endl;
	std::cout << "CF_tree.model" << std::endl;
	std::cout << "    accuracy : " << 1.0 * correct[1] / n << std::endl;

	system("pause");
	return 0;
}
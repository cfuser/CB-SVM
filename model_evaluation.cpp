#include "svm.h"
#include <iostream>
int main()
{
	svm_model *standard_model = svm_load_model("standard_train.model");
	svm_model * CF_model = svm_load_model("CF_mode");
	FILE *fp = fopen("test.txt", "r");
	int n, m;
	fscanf(fp, "%d %d", &n, &m);
	int correct_standard, correct_CF;
	for (int i = 0; i < n; i++)
	{
		svm_node *test_node = new svm_node[m + 1];
		for (int j = 0; j < m; j++)
		{
			test_node[j].index = j + 1;
			fscanf(fp, "%lf", &test_node[j].value);
		}
		test_node[m].index = -1;
		int test_type_standard = svm_predict(standard_model, test_node);
		int test_type_CF = svm_predict(CF_model, test_node);

		int correct_type;
		fscanf(fp, "%d", correct_type);
		correct_standard = correct_standard + test_type_standard == correct_type;
		correct_CF = correct_CF + test_type_CF == correct_type;
	}

	FILE *out = fopen("evaluation.txt", "w");
	fprintf(out, "total sample = ", n);
	fprintf(out, "correct_standard = %d, precent = %lf", correct_standard, 1.0 * correct_standard / n);
	fprintf(out, "correct_CF = %d, precent = %lf", correct_CF, 1.0 * correct_CF / n);
	std::cout << "model evaluation successfully" << std::endl;
	system("pause");
	return 0;
}
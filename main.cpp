#include "CF.h"
#include "CB_SVM.h"
#include "my_svm.h"
#include <vector>
#include <iostream>
#include <time.h>

CB_SVM_node *getChildren(CB_SVM_node *set, int &add_node_number)
{
	CB_SVM_node *child_node = NULL;
	CB_SVM_node *iter_node = set;
	while (iter_node)
	{
		add_node_number = add_node_number + iter_node->content->child_number - 1;
		CF_node *iter_child = iter_node->content->child;
		while (iter_child)
		{
			CB_SVM_node *set_add_node = new CB_SVM_node;
			set_add_node->content = iter_child;
			set_add_node->next = child_node;
			set_add_node->label = iter_node->label;
			child_node = set_add_node;
			iter_child = iter_child->next;
		}
		iter_node = iter_node->next;
	}
	return child_node;
}
double getDistance(svm_node *node,SVM_Solver h)
{
	double sum = 0;
	svm_node *iter_node = node;
	for (int i = 0; i < h.dimension; i++)
	{
		sum = sum + h.hyperplane[i] * iter_node->value;
		iter_node++;
	}
	sum = std::abs(sum);
	return sum / h.sqrt_w;
}
double getMaxDistanceOfSVs(SVM_Solver h)
{
	double maxDis = -DBL_MAX;
	for (int i = 0; i < h.model->l; i++)
	{
		double tempDis = getDistance(h.model->SV[i], h);
		maxDis = std::max(maxDis, tempDis);
	}
	return maxDis;
}
double getDistance(CB_SVM_node *node, SVM_Solver h)
{
	double sum = 0;
	for (int i = 0; i < h.dimension; i++)
	{
		sum = sum + h.hyperplane[i] * node->content->LS[i] / node->content->n;
	}
	sum = std::abs(sum);
	return sum / h.sqrt_w;
}
std::vector<CB_SVM_node*> getLowMargin(SVM_Solver h, CB_SVM_node *set)
{
	//vector[0] means non-devide, vector[1] means devide, vector[2] means the last node of non-devide
	std::vector<CB_SVM_node*> devide{ NULL, NULL, NULL };
	double dis = getMaxDistanceOfSVs(h);
	CB_SVM_node *iter_node = set, *record_node = NULL;
	while (iter_node)
	{
		record_node = iter_node->next;
		double temp_dis = getDistance(iter_node, h);
		double Radius = iter_node->content->Radius;
		
		if (temp_dis - Radius > dis || iter_node->content->child_number == 0)
		{
			iter_node->next = devide[0];
			devide[0] = iter_node;
			if (devide[2] == NULL)
				devide[2] = devide[0];
		}
		else
		{
			iter_node->next = devide[1];
			devide[1] = iter_node;
			//devide[0].push_back(iter_node);
		}
		iter_node = record_node;
	}
	
	return devide;
}
int main()
{
	clock_t start, finish;

	//input data
	int n, m;
	FILE *fp = fopen("train.txt", "r");
	if (fp == NULL)
	{
		printf("can't find the data.txt file, please new\n");
		return 0;
	}
	FILE *out = fopen("CF-tree.txt", "w");
	fscanf(fp, "%d %d", &n, &m);

	double *data = new double[n * m];
	int *label = new int[n];

	double threshold;
	int max_branch;
	printf("threshold = "); scanf("%lf", &threshold);
	printf("max_branch = "); scanf("%d", &max_branch);
	
	start = clock();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			fscanf(fp, "%lf", &data[i * m + j]);
		}
		fscanf(fp, "%d", &label[i]);
		label[i] = (label[i] + 1) / 2;
	}

	//BIRCH
	CF_node *root[2];
	root[0] = new CF_node(m, threshold, max_branch);
	root[1] = new CF_node(m, threshold, max_branch);
	
	for (int i = 0; i < n; i++)
	{
		Eigen::VectorXd record;
		record.resize(m);
		for (int j = 0; j < m; j++)
			record[j] = data[i * m + j];
		//record[m - 1] = (double)label[i];
		int Label = label[i];
		root[Label]->child = root[Label]->child->insert(root[Label], record);
		if (root[Label]->child_number > max_branch)
		{
			//std::cout << "root split" << std::endl;
			root[Label]->split();
			CF_node *new_root = new CF_node(m, threshold, max_branch);
			new_root->child = root[Label];
			new_root->child_number = 2;
			root[Label] = new_root;
		}
		//std::cout << i << " " << root->child->n << std::endl;
	}
	//std::cout << root->child_number << std::endl;
	
	/*
	root[0]->print(out);
	fprintf(fp, "\n");
	root[1]->print(out);
	fprintf(fp, "\n");
	*/
	//return 0;
	//std::cout << root[0]->child_number << " " << root[1]->child_number << std::endl;

	// CB-SVM
	CB_SVM_node *train_set = NULL;
	int sample_number = 0;
	for (int i = 0; i < 2; i++)
	{
		CF_node *iter_node = root[i]->child;
		while (iter_node)
		{
			//std::cout << i << std::endl;
			CB_SVM_node *set_add_node = new CB_SVM_node;
			set_add_node->content = iter_node;
			set_add_node->next = train_set;
			set_add_node->label = 2 * i - 1;
			train_set = set_add_node;
			iter_node = iter_node->next;
			sample_number = sample_number + 1;
		}
	}

	svm_model *final;
	while (true)
	{
		std::cout << std::endl << "sapmle number = " << sample_number << std::endl;
		// process: h = SVM.train(S);
		SVM_Solver svm_solver;
		svm_solver.dimension = m;
		svm_solver.prob.l = sample_number;
		svm_solver.prob.x = new svm_node*[sample_number];
		svm_solver.prob.y = new double[sample_number];
		CB_SVM_node *iter_node = train_set;

		for (int i = 0; i < sample_number; i++)
		{
			svm_solver.prob.x[i] = new svm_node[m + 1];
			for (int j = 0; j < m; j++)
			{
				svm_solver.prob.x[i][j].index = j + 1;
				svm_solver.prob.x[i][j].value = iter_node->content->LS[j] / iter_node->content->n;
			}
			svm_solver.prob.x[i][m].index = -1;
			svm_solver.prob.y[i] = iter_node->label;
			iter_node = iter_node->next;
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

		iter_node = train_set;
		for (int i = 0; i < sample_number; i++)
		{
			//std::cout << iter_node->content->n << " " << iter_node->label << std::endl;
			iter_node = iter_node->next;
		}
		svm_solver.model = svm_train(&svm_solver.prob, &svm_solver.param);
		//std::cout << svm_solver.model << std::endl;
		//svm_save_model("CF_tree.model", svm_solver.model);

		svm_solver.setHyperplane();

		std::vector<CB_SVM_node *> devide_set = getLowMargin(svm_solver, train_set);
		int add_node_number = 0;
		//std::cout << "devide successfully" << std::endl;
		devide_set[1] = getChildren(devide_set[1], add_node_number);
		//std::cout << add_node_number << std::endl;
		if (devide_set[1] == NULL)
		{
			final = svm_solver.model;
			break;
		}
		else
		{
			if (devide_set[2] == NULL)
			{
				train_set = devide_set[1];
			}
			else
			{
				devide_set[2]->next = devide_set[1];
				train_set = devide_set[0];
			}
			sample_number = sample_number + add_node_number;
		}
		//std::cout << "work" << std::endl;
		//std::cout << sample_number << std::endl;
		//system("pause");
	}

	//std::cout << "final done" << std::endl;
	finish = clock();
	svm_save_model("CF_tree.model", final);
	std::cout << "\n" << "time = " << finish - start << std::endl;
	system("pause");
	return 0;
}
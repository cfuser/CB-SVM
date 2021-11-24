#pragma once
#include "svm.h"
#include <cmath>

struct SVM_Solver
{
	double sqrt_w;
	int dimension;
	svm_problem prob;
	svm_parameter param;
	svm_model *model;
	double *hyperplane;
	double b;
	void setHyperplane()
	{
		double sqrt_w = 0;
		hyperplane = new double[dimension];
		for (int i = 0; i < model->l; i++)
		{
			double type = i < model->nSV[0]?model->label[0]:model->label[1];
			type = 2 * type - 1;
			svm_node *p = model->SV[i];
			for (int j = 0; j < dimension; j++)
			{
				hyperplane[j] = hyperplane[j] + model->sv_coef[0][i] * type * p->value;
				p++;
			}
		}
		for (int j = 0; j < dimension; j++)
		{
			sqrt_w = sqrt_w + std::pow(hyperplane[j], 2.0);
		}
		sqrt_w = std::sqrt(sqrt_w);
		b = - *model->rho;
	}
};
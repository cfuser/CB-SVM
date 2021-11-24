#pragma once
#include "Eigen/Eigen"
struct CF_node {
	int dimension;
	int n;
	Eigen::VectorXd LS, SS;
	//Eigen::VectorXd LS = Eigen::VectorXd::Zero(dimension), SS = Eigen::VectorXd::Zero(dimension);
	double Radius;
	CF_node *child;
	CF_node *next;
	int child_number;
	double threshold;
	int max_branch;
	CF_node* insert(CF_node *father, Eigen::VectorXd point);
	void split();
	CF_node(int dimension, double threshold, int max_branch){
		this->dimension = dimension;
		n = 0;
		LS = Eigen::VectorXd::Zero(dimension);
		SS = Eigen::VectorXd::Zero(dimension);
		child = NULL;
		next = NULL;
		child_number = 0;
		this->threshold = threshold;
		this->max_branch = max_branch;
	}
	
	void print(FILE *out)
	{
		CF_node *iter_node = this;
		while (iter_node)
		{
			fprintf(out, "node = %p\n",iter_node);
			fprintf(out, "n = %d\nLS = %lf, %lf\nSS = %lf, %lf\n", iter_node->n, iter_node->LS[0], iter_node->LS[1], iter_node->SS[0], iter_node->SS[1]);
			fprintf(out, "child_number = %d\n", iter_node->child_number);
			CF_node *iter_child = this->child;
			while (iter_child)
			{
				fprintf(out, "child = %p\n", iter_child);
				iter_child = iter_child->next;
			}
			fprintf(out, "- - -\n");
			iter_node->child->print(out);
			iter_node = iter_node->next;
		}
		return;
	}
};


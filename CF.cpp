#include "CF.h"
#include <vector>
#include <iostream>
void update(CF_node *node, Eigen::VectorXd data)
{
	node->n = node->n + 1;
	node->LS = node->LS + data;
	Eigen::VectorXd SS2(node->SS.size());
	for (int i = 0; i < SS2.size(); i++)
		SS2[i] = std::pow(data[i], 2);
	node->SS = node->SS + SS2;
}
void update(CF_node *node, CF_node *union_node)
{
	node->n = node->n + union_node->n;
	node->LS = node->LS + union_node->LS;
	node->SS = node->SS + union_node->SS;
}
void add_node(CF_node *node, CF_node *add_node)
{
	add_node->next = node->child;
	node->child = add_node;
	node->child_number = node->child_number + 1;
}
void del_node(CF_node *node, CF_node *del_node)
{

}
double get_dis(CF_node *node, Eigen::VectorXd data)
{
	Eigen::VectorXd dis = node->LS / node->n - data;
	//std::cout << "\n" << dis << "\n" << std::endl;
	return dis.norm();
}
double get_dis(CF_node *first_node, CF_node *second_node)
{
	Eigen::VectorXd dis = first_node->LS / first_node->n - second_node->LS / second_node->n;
	return dis.norm();
}
double get_new_threshold(CF_node *node, Eigen::VectorXd data)
{
	CF_node *assume_new_node = new CF_node(node->dimension, node->threshold, node->max_branch);
	assume_new_node->n = node->n;
	assume_new_node->LS = node->LS;
	assume_new_node->SS = node->SS;
	update(assume_new_node, data);
	//std::cout << assume_new_node->n << " " << assume_new_node->LS << " " << assume_new_node->SS << std::endl;
	Eigen::VectorXd LS2(assume_new_node->LS.size());
	for (int i = 0; i < LS2.size(); i++)
		LS2[i] = std::pow(assume_new_node->LS[i], 2);
	Eigen::VectorXd dis = (assume_new_node->SS - LS2 / assume_new_node->n) / assume_new_node->n;
	node->Radius = dis.norm();
	return dis.norm();
}
std::vector<CF_node*> max_dis(CF_node *node)
{
	double dis = -DBL_MAX;
	CF_node *first_child = node->child;
	std::vector<CF_node*> devide(2);
	while (first_child)
	{
		CF_node *second_child = first_child->next;
		while (second_child)
		{
			double temp_dis = get_dis(first_child, second_child);
			if (temp_dis > dis)
			{
				dis = temp_dis;
				devide[0] = first_child;
				devide[1] = second_child;
			}
			second_child = second_child->next;
		}
		first_child = first_child->next;
	}
	return devide;
}

void CF_node::split()
{
	CF_node *min_node = this;
	CF_node *new_node = new CF_node(dimension, threshold, max_branch);
	std::vector<CF_node*> devide_node = max_dis(min_node);
	new_node->next = min_node->next;
	min_node->next = new_node;

	//devide child to the closest devide_node
	/*
	min_node->n = devide_node[0]->n;
	min_node->LS = devide_node[0]->LS;
	min_node->SS = devide_node[0]->SS;

	new_node->n = devide_node[1]->n;
	new_node->LS = devide_node[1]->LS;
	new_node->SS = devide_node[1]->SS;
	*/
	min_node->n = 0;
	min_node->LS = Eigen::VectorXd::Zero(dimension);
	min_node->SS = Eigen::VectorXd::Zero(dimension);

	CF_node *min_child = min_node->child;
	min_node->child = NULL;
	min_node->child_number = 0;
	CF_node *next_record = NULL;
	while (min_child)
	{
		/*
		if (min_child == devide_node[0] || min_child == devide_node[1])
		{
			min_child = min_child->next;
			continue;
		}
		*/
		next_record = min_child->next;
		double dis[2] = { get_dis(devide_node[0], min_child), get_dis(devide_node[1], min_child) };
		//if (get_dis(devide_node[0], min_child) < get_dis(devide_node[1], min_child))
		if (dis[0] < dis[1])
		{
			update(min_node, min_child);
			add_node(min_node, min_child);
		}
		else
		{
			update(new_node, min_child);
			add_node(new_node, min_child);
		}
		min_child = next_record;
	}
	return;
}
//root_child to add node
CF_node* CF_node::insert(CF_node *father, Eigen::VectorXd data)
{
	CF_node *iter_node = this;
	
	if (iter_node == NULL)
	{
		CF_node *new_node = new CF_node(father->dimension, father->threshold, father->max_branch);
		update(new_node, data);
		iter_node = new_node;
		father->child_number = father->child_number + 1;
		return iter_node;
	}
	// find the closest leaf cluster
	CF_node *min_node = NULL;
	double min_dis = DBL_MAX;
	
	
	while (iter_node)
	{
		double temp_dis = get_dis(iter_node, data);
		if (temp_dis < min_dis)
		{
			min_node = iter_node;
			min_dis = temp_dis;
		}
		iter_node = iter_node->next;
	}
	
	// modify the leaf
	
	if (min_node->child == NULL)
	{
		if (get_new_threshold(min_node, data) <= threshold)
		{
			update(min_node, data);
		}
		else
		{
			CF_node *new_node = new CF_node(dimension, threshold, max_branch);
			update(new_node, data);
			new_node->next = min_node->next;
			min_node->next = new_node;
			father->child_number = father->child_number + 1;

			//	father->add_child(new_node);
		}
		return this;
	}
	else
	{
		update(min_node, data);
		min_node->child = min_node->child->insert(min_node, data);
		if (min_node->child_number > max_branch)
		{
			//std::cout << "split" << std::endl;
			father->child_number = father->child_number + 1;
			min_node->split();
		}
	}
	return this;
}
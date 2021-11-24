#pragma once
#include "CF.h"

struct CB_SVM_node
{
	CF_node *content;
	CB_SVM_node *next;
	int label;
	CB_SVM_node()
	{
		content = NULL;
		next = NULL;
		label = -1;
	}
};
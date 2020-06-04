#include "ProductGroup.h"



ProductGroup::ProductGroup(string name, vector <User> exp, vector <Product> prod)
{
	this->name = name;
	//this->products = prod;



	//this->experts = exp;
	this->maxOfScale = products.size()*(products.size() - 1);
}


ProductGroup::~ProductGroup()
{
}

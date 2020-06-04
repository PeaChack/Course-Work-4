#pragma once
#include "Product.h"
#include "User.h"
#include <map>
class ProductGroup
{
	bool readyForReport = false;
	int maxOfScale;
	string name;
	vector<Product> products;
	vector<User> experts;
public:
	ProductGroup(string name,vector <User>,vector <Product>);
	~ProductGroup();
};


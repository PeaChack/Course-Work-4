#pragma once
#include <string>
#include <vector>
using namespace std;
class Product
{
	int productID;
	string name;
	double price;
	double storageCost;
	int sellByDays;
	double nonRealizedPercent;
	bool operator ==(Product u) { return (productID == u.productID); }
public:
	Product();
	~Product();
};


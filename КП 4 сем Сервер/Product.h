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
public:
	string getName() const { return name; }
	int getID() const { return productID; }
	double getPrice() const { return price; }
	double getStorageCost() const { return storageCost; }
	double getPrice()  { return price; }
	double getStorageCost()  { return storageCost; }
	int getSellByDays() const { return sellByDays; }
	double getNonRealizedPercent() const { return nonRealizedPercent; }
	void setName(string n) { this->name = n; }
	void setID(int n) { this->productID = n; }
	void setPrice(double n) { this->price = n; }
	void setStorageCost(double n) { this->storageCost = n; }
	void setSellByDays(int n) { this->sellByDays = n; }
	void setNonRealizedPercent(double n) { this->nonRealizedPercent = n; }
	bool operator==(Product p) { return (this->productID == p.productID); }
	bool operator<(Product p) const { return (this->productID < p.productID); }
	string getFullInfo() const;
	string commaToDot(string s) const
	{
		if (s.find(',')!= string::npos)
		s[s.find(',')] = '.';
		return s;
	}
	string cutStrDouble(string s) const
	{
		unsigned int pos;
		if ((pos = s.find('.') + 4) < s.length())
			s.erase(pos, s.length() - pos);
		return s;
	}
	Product();
	Product(int);
	~Product();
};


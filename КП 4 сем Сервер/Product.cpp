#include "Product.h"



Product::Product()
{
	name = "";
	price = 0.0;
	storageCost = 0.0;
	sellByDays = 0;
	nonRealizedPercent = 0;
}

Product::Product(int ID)
{
	productID = ID;
	name = "";
	price = 0.0;
	storageCost = 0.0;
	sellByDays = 0;
	nonRealizedPercent = 0;
}

string Product::getFullInfo() const
{
	string result;
	result += "Название: " + getName() + "\nID: " + to_string(getID()) + "\nЦена: " + cutStrDouble(commaToDot(to_string(getPrice()))) + " руб.\nСтоимость хранения: " + cutStrDouble(commaToDot(to_string(getStorageCost())));
	result += " руб.\nСрок реализации: " + to_string(getSellByDays()) + " дней\nПроцент списаний: " + cutStrDouble(commaToDot(to_string(getNonRealizedPercent()))) + "%\n";
	return result;
}

Product::~Product()
{
}

#include "ProductGroup.h"



ProductGroup::ProductGroup(string name, int scale, vector <User> exp, vector <Product> prod)
{
	this->name = name;
	this->maxOfScale = scale;
	for (auto &iter : prod)
	{
		products.insert(pair < Product, double >(iter, 0.0));
	}
	for (auto &iter : exp)
	{
		vector <double> a;
		experts.insert(pair<User, vector<double>>(iter, a));
	}
}

ProductGroup::ProductGroup(string name, int scale, map <User,vector<double>> exp, vector <Product> prod)
{
	this->name = name;
	this->maxOfScale = scale;
	for (auto &iter : prod)
	{
		products.insert(pair < Product, double >(iter, 0.0));
	}
	this->experts = exp;
}

ProductGroup::~ProductGroup()
{
}

string ProductGroup::getExpertsForPrint()
{
	string result = "";
	for (auto &iter : experts)
	{
		result += iter.first.getLName() + " " + iter.first.getFName() + " (" + iter.first.getLogin() + ") Статус оценки: " + ((iter.second.size() != 0) ? "Выставлена" : "Не выставлена") + "\n";
	}
	return result;
}
string ProductGroup::getProductsForPrint()
{
	string result;
	for (auto &iter : products)
	{
		result += iter.first.getName() + "\n";
	}
	return result;
}

void ProductGroup::checkReady()
{
	readyForReport = true;
	for (auto &iter : experts)
	{
		if (iter.second.size() == 0)
			readyForReport = false;
	}
	if (readyForReport)
	{
		int numProd = 0;
		for (auto &iter : products)
		{
			double markSum = 0;
			for (auto &iter1 : experts)
			{
				markSum += iter1.second[numProd];
			}
			iter.second = markSum / (products.size()*(products.size() - 1)) * 2 / experts.size();
			numProd++;
		}
	}
}

string ProductGroup::getReport()
{
	string result;
	if (readyForReport == false)
		return "Отчёт не готов.";
	else
	{
		result = "Отчёт по группе товаров " + name + "\nОтветственные за оценку:\n";
		for (auto &iter : experts)
			result += iter.first.getLName() + " " + iter.first.getFName() + " (" + iter.first.getLogin() + ")\n";
		result += "В ходе выставления оценок были выявлены следующие веса товаров\n";
		for (auto &iter : products)
		{
			result += "Продукт " + iter.first.getName() + " (id " + to_string(iter.first.getID()) + "): " + iter.first.cutStrDouble(iter.first.commaToDot(to_string(round(iter.second*1000)/1000))) + "\n";
		}
		return result;
	}
}
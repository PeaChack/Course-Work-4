#include "opDatabase.h"



opDataBase::opDataBase()
{
}


opDataBase::~opDataBase()
{
}

void opDataBase::saveUsers()
{
	fstream file;
	file.open("users.txt", ios_base::out);
	for (auto &iter : userVector)
	{
		file << endl << iter.getFName() << endl << iter.getLName() << endl << iter.getAdminMode() << endl;
		file << iter.getLogin() << endl << iter.getPassword();
	}
	file.close();
}
void opDataBase::saveProducts()
{
	fstream file;
	file.open("products.txt", ios_base::out);
	for (auto &iter : productVector)
	{
		file << endl << iter.getID() << endl << iter.getName() << endl << iter.getNonRealizedPercent() << endl << iter.getPrice() << endl << iter.getSellByDays() << endl << iter.getStorageCost();
	}
	file.close();
}

void opDataBase::saveProductGroups()
{
	fstream file;
	file.open("product groups.txt", ios_base::out);
	int size = sizeof(prodGroupVector);
	
	for (auto &iter : prodGroupVector)
	{
		file << endl << iter.name << endl << iter.readyForReport << ' ' << iter.maxOfScale << ' ' << iter.experts.size() << endl;
		for (auto &iter1 : iter.experts)
		{
			file << iter1.first.getFName() << ' ' << iter1.first.getLName() << endl << iter1.first.getLogin() << endl;
			file << iter1.second.size();
			for (auto &iter2 : iter1.second)
			{
				file << ' ' << iter2;
			}
			file << ' ';
		}
		file << iter.products.size();
		for (auto &iter1 : iter.products)
		{
			file << endl << iter1.first.getName() << endl << iter1.first.getID() << ' ' << iter1.first.getNonRealizedPercent() << ' ' << iter1.first.getPrice() << ' ' << iter1.first.getSellByDays() << ' ' << iter1.first.getStorageCost();
		}
	}
	file.close();
}

void opDataBase::saveAll()
{
	saveUsers();
	saveProducts();
	saveProductGroups();
}

void opDataBase::uploadUsers()
{
	string tempS;
	bool tempB;
	User tempU;
	fstream file;
	file.open("users.txt", ios_base::in);
	if (file.is_open() && file.peek() != EOF)
	{
		getline(file, tempS);
		while (file.peek() != EOF)
		{
			getline(file, tempS);
			tempU.setFName(tempS);
			getline(file, tempS);
			tempU.setLName(tempS);
			file >> tempB;
			tempU.setAdminMode(tempB);
			getline(file, tempS);
			getline(file, tempS);
			tempU.setLogin(tempS);
			getline(file, tempS);
			tempU.setPassword(tempS);
			userVector.push_back(tempU);
		}
	}
	else
		userVector.push_back(User("admin", "root", 1));

	file.close();
}

void opDataBase::uploadProducts()
{
	fstream file;
	string tempS;
	Product tempP;
	file.open("products.txt", ios_base::in);
	if (file.is_open() && file.peek() != EOF)
	{
		getline(file, tempS);
		while (file.peek() != EOF)
		{
			getline(file, tempS);
			tempP.setID(stoi(tempS));
			getline(file, tempS);
			tempP.setName(tempS);
			getline(file, tempS);
			//tempS[tempS.find(',')] = '.';
			tempP.setNonRealizedPercent(stod(tempS));
			getline(file, tempS);
			//tempS[tempS.find(',')] = '.';
			tempP.setPrice(stod(tempS));
			getline(file, tempS);
			tempP.setSellByDays(stoi(tempS));
			getline(file, tempS);
			//tempS[tempS.find(',')] = '.';
			tempP.setStorageCost(stod(tempS));
			productVector.push_back(tempP);
		}
	}
	file.close();
}
void opDataBase::uploadProductGroups()
{
	fstream file;
	file.open("product groups.txt", ios_base::in);
	int tempScale , count, tempI;
	double tempD;
	bool tempB;
	User tempU;
	vector<Product> tempPV;
	map<User, vector<double>> experts;
	string tempS, tempName;
	if (file.is_open() && file.peek() != EOF)
	{
		getline(file, tempS);
		while (file.peek() != EOF)
		{
			getline(file, tempName);
			file >> tempB >> tempScale >> count;
			getline(file, tempS);
			for (int i = 0; i < count; i++)
			{
				int markCount;
				file >> tempS;
				tempU.setFName(tempS);
				file >> tempS;
				tempU.setLName(tempS);
				getline(file, tempS);
				getline(file, tempS);
				tempU.setLogin(tempS);
				file >> markCount;
				vector<double> marks;
				for (int j = 0; j < markCount; j++)
				{
					file >> tempD;
					marks.push_back(tempD);
				}
				experts.insert(pair<User, vector<double>>(tempU, marks));
				marks.clear();
			}
			file >> count;
			getline(file, tempS);
			for (int i = 0; i < count; i++)
			{
				Product p;
				getline(file, tempS);
				p.setName(tempS);
				file >> tempI;
				p.setID(tempI);
				file >> tempD;
				p.setNonRealizedPercent(tempD);
				file >> tempD;
				p.setPrice(tempD);
				file >> tempI;
				p.setSellByDays(tempI);
				file >> tempD;
				getline(file, tempS);
				p.setStorageCost(tempD);
				tempPV.push_back(p);
			}
			prodGroupVector.push_back(ProductGroup(tempName, tempScale, experts, tempPV));
			prodGroupVector.back().checkReady();
			experts.clear();
			tempPV.clear();
		}
	}
	file.close();
}
void opDataBase::uploadAll()
{
	uploadUsers();
	uploadProducts();
	uploadProductGroups();
}
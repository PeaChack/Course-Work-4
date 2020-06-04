#pragma once
#include <vector>
#include "ProductGroup.h"
#include <fstream>
using namespace std;
class opDataBase
{
	opDataBase();
	vector<User> userVector;// все пользователи
	vector<User> activeUsers;// активные пользователи
	vector<Product> productVector;//все продукты
	vector<ProductGroup> prodGroupVector;//все группы товаров
public:
	static opDataBase& Hinstance()
	{
		static opDataBase Obj;
		return Obj;
	}
	void saveAll();	
	void saveUsers ();
	void saveProducts();
	void saveProductGroups();
	void uploadUsers();
	void uploadProducts();
	void uploadProductGroups();
	void uploadAll();
	~opDataBase();
	friend class ClientInterface;
};


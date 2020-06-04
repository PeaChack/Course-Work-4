#pragma once
#include "ProductGroup.h"
class Expert : public User
{
	map<ProductGroup, bool> listGroups;
public:
	bool operator==(User u);
	void loadUncheckedGr();
	Expert(User &u)
	{
		this->setLogin(u.getLogin());
		this->setPassword(u.getPassword());
		this->setAdminMode(u.getAdminMode());
	}
	Expert();
	~Expert();
};


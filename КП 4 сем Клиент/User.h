#pragma once
#include "Person.h"
class User : public Person 
{
	string login;
	string password;
	bool adminMode;
	bool is_rated = false;
public:
	string getLogin() { return login; }
	string getPassword() { return password; }
	bool getAdminMode() { return adminMode; }
	void setLogin(string log) { this->login = log; }
	void setPassword(string pass) { this->password = pass; }
	void setAdminMode(bool adm) { this->adminMode = adm; }
	bool operator==(User u);
	User();
	User(string login, string password, bool adminMode = 0);
	~User();
};


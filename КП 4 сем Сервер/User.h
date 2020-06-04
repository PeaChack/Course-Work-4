#pragma once
#include "Person.h"
class User : public Person
{
	string login;
	string password;
	bool adminMode;
public:
	string getLogin() const { return login; }
	string getPassword() const { return password; }
	bool getAdminMode() const { return adminMode; }
	void setLogin(string log) { this->login = log; }
	void setPassword(string pass) { this->password = pass; }
	void setAdminMode(bool adm) { this->adminMode = adm; }
	bool operator==(User u);
	bool operator<(User u) const { return (this->login < u.login); }
	User();
	User(string login, string password, bool adminMode = 0);
	~User();
};


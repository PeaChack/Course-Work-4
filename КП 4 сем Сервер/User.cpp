#include "User.h"


User::User()
{
	login = "";
	password = "";
	adminMode = 0;
}
User::User(string login, string password, bool adminMode)
{
	this->login = login;
	this->password = password;
	this->adminMode = adminMode;
}

bool User::operator==(User u)
{
	return (this->login == u.login && this->password == u.password);
}

User::~User()
{
}

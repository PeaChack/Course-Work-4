#pragma once
#include <string>
using namespace std;
class Person
{
	string firstName;
	string lastName;
public:
	string getFName() { return firstName; }
	string getLName() { return lastName; }
	void setFName(string fname) { this->firstName = fname; }
	void setLName(string lname) { this->lastName = lname; }
	Person();
	~Person();
};


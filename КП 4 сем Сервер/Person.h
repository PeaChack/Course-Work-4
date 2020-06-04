#pragma once
#include <string>
#include <map>
using namespace std;
class Person
{
	string firstName;
	string lastName;
public:
	string getFName() const { return firstName; }
	string getLName() const { return lastName; }
	void setFName(string fname) { this->firstName = fname; }
	void setLName(string lname) { this->lastName = lname; }
	Person();
	~Person();
};


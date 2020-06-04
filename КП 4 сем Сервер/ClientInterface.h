#pragma once
#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "opDatabase.h"

class ClientInterface
{	
	User currentUser;
	string currentCommand;
	SOCKET client_socket;
public:
	ClientInterface(SOCKET);
	~ClientInterface();
	string commaToDot(string);
	void sendToClient(string str);
	string recvFromClient(bool hideLog = false);
	void startMenu();
	void auth();
	void registration();
	void adminMenu();
	void userMenu();
	void userControlMenu();
	void productsControlMenu();
	void printFullReport();
	void printUsers();
	void addUser();
	void editUser();
	void deleteUser();
	void printGroups();
	void addProdGroup();
	void delProdGroup();
	void printProducts();
	void addProduct();
	void delProduct();
	void expertRateMenu();
};

class BREAK_THREAD_EX
{
	string what;
public:
	BREAK_THREAD_EX(string wh)
	{
		what = wh;
	}
	string what_is() { return what; }
};


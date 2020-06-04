#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iomanip>
#include <ctime>
#include <fstream>
#include "User.h"
using namespace std;
class SClient
{
	string serverMsg;
	User currentUser;
	SOCKET server_socket;
	unsigned short server_port;
public:
	void connectToS();
	void sendToS(string str);
	string recvFromS();
	int getMenuChoice(int countOfMenuBoxes, int numberOfFirst = 0);
	template <typename T>
	T getNum();
	void auth();
	void startMenu();
	void registration();
	bool adminMenu();
	bool userMenu();
	void userControlMenu();
	void productsControlMenu();
	void printFullReport();
	void programInfo();
	void printUsers();
	void addUser();
	void editUser();
	void deleteUser();
	void infoGroups();
	void addProdGroup();
	void delProdGroup();
	void printProducts();
	void addProduct();
	void delProduct();
	void expertRateMenu();
	void saveData();
	SClient();
	~SClient();
};


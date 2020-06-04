#include <iostream>
#include "SServer.h"
#include <Windows.h>
using namespace std;
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	SServer server;
	server.setPort(1024);
	server.startServer();
}	
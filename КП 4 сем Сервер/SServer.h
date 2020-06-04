#pragma once
#include "ClientInterface.h"
#include <chrono>
class SServer
{
	unsigned short port;
	unsigned short client_count;
public:
	SServer();
	~SServer();
	void setPort(unsigned short port);
	void startServer();
	void closeServer();
	void autoSave();
	void handle(SOCKET client_socket);
	SOCKET s;
	WSAData wData;
};


#pragma comment(lib, "ws2_32.lib")
#include "SServer.h"
using namespace std;


SServer::SServer()
{
	client_count = 0;
}

void SServer::autoSave()
{
	while (true)
	{
		this_thread::sleep_for(chrono::seconds(60));
		opDataBase::Hinstance().saveAll();
		cout << "Автосохранение сервера..." << endl;
	}
}

SServer::~SServer()
{
}

void SServer::startServer() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) /*return 0*/; // тут терминатора бы
	s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);
	local_addr.sin_addr.s_addr = 0;
	bind(s, (sockaddr*)&local_addr, sizeof(local_addr));
	if (s == SOCKET_ERROR) {
		cout << "Сокет не был создан\n";
	}

	if (::bind(s, (sockaddr*)&local_addr, sizeof(local_addr)) != SOCKET_ERROR) {
		printf("Сокет успешно привязан\n");
	}

	if (listen(s, 5) != SOCKET_ERROR) {
		printf("Старт прослушивания порта: %u\n", ntohs(local_addr.sin_port));
	}
	SOCKET client_socket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	opDataBase::Hinstance().uploadAll();
	thread save(&SServer::autoSave, this);
	save.detach();
	while (client_socket = accept(s, (sockaddr*)&client_addr, &client_addr_size))
	{
		client_count++;
		cout << "Подключился новый клиент!" << endl;
		cout << "Общее число клиентов: " << client_count << endl;
		thread th(&SServer::handle, this , client_socket);
		th.detach();
	}
}

void SServer::closeServer() {
	closesocket(s);
	WSACleanup();
	cout << "Сервер был остановлен." << endl;
}


void SServer::handle(SOCKET client_socket) {
	ClientInterface clInterf(client_socket);
	cout << "Поток " << this_thread::get_id() << " начал работу с клиентом." << endl;
	try {
		clInterf.startMenu();
	}
	catch (BREAK_THREAD_EX ex)
	{
		cout << "Поток " << setw(5) << this_thread::get_id() << " : " << ex.what_is() << endl;
	}
	closesocket(client_socket);
	client_count--;
	cout << "Сохранение изменений пользователя из потока " << this_thread::get_id() << "..." << endl;
	opDataBase::Hinstance().saveAll();
	cout << "Поток " << this_thread::get_id() << " завершил работу." << endl;
	cout << "Общее число клиентов: " << client_count << endl;
}


void SServer::setPort(unsigned short port)
{
	this->port = port;
}
#include "SClient.h"
#pragma warning(disable:4996)
#define BREAK_THREAD 1


SClient::SClient()
{
	serverMsg = "";
}


SClient::~SClient()
{
}

void SClient::programInfo()
{
	system("cls");
	system("title О программе");
	cout << "Данная программа была создана в качестве курсового проекта \nпо дисциплине \"системный анализ и проектирование информационных систем\" \nна тему «Программное средство для принятия решений методом \nполного попарного сравнения (на примере торгового предприятия)»\nВ качестве примера торгового предприятия был взят магазин подарков \"Инь-янь\"\n\nАвтор курсового проекта: Булыга Дмитрий Игоревич\nгруппа 872302, БГУИР" << endl;
	system("pause");
}

int SClient::getMenuChoice(int countOfMenuBoxes, int numberOfFirst)
{
	int choice;
	cout << "Ваш выбор: ";
	while (!(cin >> choice) || choice < numberOfFirst || choice > (numberOfFirst + countOfMenuBoxes - 1))
	{
		cout << "Ошибка ввода! Повторите попытку: ";
		cin.clear();
		rewind(stdin);
	}
	return choice;
}
template <typename T>
T SClient::getNum()
{
	T choice;
	while (!(cin >> choice))
	{
		cout << "Ошибка ввода! Повторите попытку: ";
		cin.clear();
		rewind(stdin);
	}
	return choice;
}

void SClient::connectToS()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0);
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(1024);
	dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while (connect(server_socket, (sockaddr*)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR)
	{
		cout << "Ошибка соединения с сервером, проверьте статус сервера и повторите попытку" << endl;
		system("pause");
	}
	try {
		startMenu();
	}
	catch (int)
	{
		cout << "Сервер оборвал соединение.\n";
		system("pause");
	}
	closesocket(server_socket);
}
void SClient::sendToS(string str)
{
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	for (unsigned int i = 0; i < (str.length() / (BUFFER_SIZE-1)); i++)
	{
		buf[0] = '1';
		str.copy(buf + 1, BUFFER_SIZE-1, i * (BUFFER_SIZE-1));
		if (send(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD;
	}
	buf[0] = '0';
	str.copy(buf + 1, BUFFER_SIZE-1, str.length() / (BUFFER_SIZE - 1));
	buf[str.length() % (BUFFER_SIZE - 1)+1] = '\0';
	if (send(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD;

}
string SClient::recvFromS()
{
	string result = "";
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	if (recv(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD;
	result += buf + 1;
	while (buf[0] == 1)
	{
		if (recv(server_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD;
		result += buf + 1;
	}
	return result;

}
void SClient::startMenu()
{
	bool back = 0;
	while (!back)
	{
		system("cls");
		system("title Стартовое меню");
		cout << "Добро пожаловать в приложение магазина \"Инь-янь\"" << endl;
		cout << "1. Авторизация\n2. Регистрация\n0. Выход\n";
		switch (getMenuChoice(3))
		{
		case 1:
			auth();
			break;
		case 2:
			registration();
			break;
		case 0:
			sendToS("Exit");
			back = 1;
			break;
		}
	}
}
void SClient::auth()
{	
	bool back = 0;
	sendToS("Start authorization");
	while (!back) {
		system("cls");
		system("title Авторизация");
		string temp;
		cout << "Добро пожаловать в меню авторизации!\n" << endl;
		cout << "Введите ваш логин: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		currentUser.setLogin(temp);
		sendToS(temp);
		cout << "Введите пароль: ";
		getline(cin, temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Not founded")
		{
			cout << "Ошибка авторизации! Неверный логин или пароль. Желаете продолжить?\n1. Да\n0. Нет\n";
			if (!getMenuChoice(2))
				back = true;
		}
		else
		{
			currentUser.setAdminMode(stoi(recvFromS()));
			currentUser.setFName(recvFromS());
			currentUser.setLName(recvFromS());
			cout << "Авторизация прошла успешно!" << endl;
			if (currentUser.getFName() != "")
				cout << "Рады снова вас видеть, " << currentUser.getFName() << "!\n";
			system("pause");
			if (currentUser.getAdminMode())
				back = adminMenu();
			else
				back = userMenu();
		}
	}
	sendToS("Back");
}

void SClient::registration()
{
	sendToS("Start registration");
	while (true)
	{
		system("title Регистрация");
		system("cls");
		string temp;
		cout << "Введите фамилию: ";
		cin >> temp;
		currentUser.setLName(temp);
		sendToS(temp);
		cout << "Введите имя: ";
		cin >> temp;
		sendToS(temp);
		currentUser.setFName(temp);
		cout << "Введите логин: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setLogin(temp);
		cout << "Введите пароль: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		currentUser.setPassword(temp);
		rewind(stdin);
		serverMsg = recvFromS();
		if (serverMsg == "Already exist")
		{
			cout << "Пользователь с таким логином и паролем уже существует!\nПовторить попытку?\n1. Да\n0. Нет" << endl;
			if (!getMenuChoice(2))
				break;
		}
		if (serverMsg == "Successful registration")
		{
			cout << "Регистрация проведена успешно!" << endl;
			system("pause");
			break;
		}
	}
	sendToS("Back");
}

bool SClient::adminMenu()
{
	bool back = false;
	sendToS("Start admin menu");
	while (!back)
	{
		system("title Меню администратора");
		system("cls");
		cout << "Добро пожаловать, администратор " << currentUser.getFName() << "\n1. Меню работы с пользователями\n2. Меню работы с товарами\n3. Вывод отчёта по всем группам товаров\n4. О программе\n5. Сохранить все данные на сервере\n0. Выход из аккаунта\n";
		switch (getMenuChoice(6))
		{
		case 1:
			userControlMenu();
			break;
		case 2:
			productsControlMenu();
			break;
		case 3:
			printFullReport();
			break;
		case 4:
			programInfo();
			break;
		case 5:
			saveData();
			break;
		case 0:
			sendToS("Back");
			back = true;

			break;
		}
	}
	return back;
}

bool SClient::userMenu()
{
	bool back = false;
	sendToS("Start user menu");
	while (!back)
	{
		system("cls");
		system("title Меню пользователя");
		cout << "Приветствуем вас, " << currentUser.getFName() << "\n1. Группы товаров, ожидающих оценки\n2. Просмотр всех товаров\n3. Просмотр всех групп товаров\n4. Создание полного отчёта\n5. О программе\n0. Выход" << endl;
		switch (getMenuChoice(6))
		{
		case 1:
			expertRateMenu();
			break;
		case 2:
			printProducts();
			break;
		case 3:
			infoGroups();
			break;
		case 4:
			printFullReport();
			break;
		case 5:
			programInfo();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
	return true;
}

void SClient::expertRateMenu()
{
	sendToS("Start expert rate menu");
	while (true) {
		system("cls");
		system("title Оценка групп");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найдена ни одна группа!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер группы для того чтобы просмотреть подробную информацию" << endl;
			cout << setw(3) << "#" << setw(20) << "Название группы" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0. Выход" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			cout << "Начать оценку группы?\n1. Да\n0. Нет" << endl;
			choice = getMenuChoice(2);
			if (choice)
			{
				sendToS("Start rate");
				int maxScale = stoi(recvFromS());
				int count = stoi(recvFromS());
				for (int i = 0; i < count; i++)
				{
					system("cls");
					for (int j = i + 1; j < count; j++)
					{
						cout << recvFromS();
						cout << recvFromS();
						cout << recvFromS() << endl;
						sendToS(to_string(getMenuChoice(maxScale)));
						system("cls");
					}
				}
				cout << recvFromS() << endl;
				system("pause");
			}
			else
			{
				break;
			}
		}
	}
	sendToS("Back");
}

void SClient::userControlMenu()
{
	bool back = 0;
	sendToS("Start user control menu");
	while (!back) {
		system("title Меню управления пользователями");
		system("cls");
		cout << "Меню управления пользователями\n1. Вывести список пользователей\n2. Создать нового пользователя\n3. Редактировать существующего пользователя\n4. Удаление пользователя\n0. Назад" << endl;
		switch (getMenuChoice(5))
		{
		case 1:
			printUsers();
			system("pause");
			break;
		case 2:
			addUser();
			break;
		case 3:
			editUser();
			break;
		case 4:
			deleteUser();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
}

void SClient::productsControlMenu()
{
	bool back = 0;
	sendToS("Start products control menu");
	while (!back)
	{
		system("cls");
		system("title Меню управления товарами");
		cout << "Меню управления товарами\n1. Просмотреть оценочные группы\n2. Добавить оценочную группу\n3. Удалить оценочную группу\n4. Просмотреть все товары\n5. Добавить товар\n6. Удалить товар\n0. Назад\n";
		switch (getMenuChoice(7))
		{
		case 1:
			infoGroups();
			break;
		case 2:
			addProdGroup();
			break;
		case 3:
			delProdGroup();
			break;
		case 4:
			printProducts();
			break;
		case 5:
			addProduct();
			break;
		case 6:
			delProduct();
			break;
		case 0:
			back = true;
			break;
		}
	}
	sendToS("Back");
}

void SClient::printFullReport()
{
	sendToS("Start print full report");
	string result;
	system("cls");
	result = "ГОТОВЫЕ ОТЧЁТЫ ПО ОЦЕНОЧНЫМ ГРУППАМ ТОВАРОВ\n\n";
	cout << result;
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		result += "Нет готовых отчётов\n";
		cout << "Нет готовых отчётов\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	result += "\nИнформация по существующим товарам:\n";
	cout << "\nИнформация по существующим товарам:\n";
	serverMsg = recvFromS();
	if (serverMsg == "End")
	{
		cout << "В базе нет товаров\n";
		result += "В базе нет товаров\n";
	}
	while (serverMsg != "End")
	{
		cout << serverMsg;
		result += serverMsg;
		serverMsg = recvFromS();
	}
	cout << "Желаете сохранить отчёт в файл?\n1. Да\n0. Нет\n";
	if (getMenuChoice(2))
	{
		time_t rawtime;
		struct tm * timeinfo;
		char buf[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buf, 80, "%d-%m-%y %Hч%Mм", timeinfo);
		system("mkdir Отчёты");
		string filename = "Отчёты\\Отчёт на ";
		filename += buf;
		filename += ".txt";
		fstream file;
		file.open(filename, ios_base::out);
		file << result;
		file.close();
	}
}

void SClient::printUsers()
{
	sendToS("Start print users");
	User temp;
	int count;
	count = stoi(recvFromS());
	cout << setw(3) << '#' << setw(10) << "Имя" << setw(10) << "Фамилия" << setw(10) << "Логин" << setw(10) << "Пароль" << setw(6) << "Админ" << endl;
	cout.fill('-');
	cout << setw(49) << '-' << endl;
	cout.fill(' ');
	for (int i = 0; i < count; i++)
	{
		temp.setFName(recvFromS());
		temp.setLName(recvFromS());
		temp.setLogin(recvFromS());
		temp.setPassword(recvFromS());
		temp.setAdminMode(stoi(recvFromS()));
		cout << setw(3) << i+1 << setw(10) << temp.getFName() << setw(10) << temp.getLName() << setw(10) << temp.getLogin() << setw(10) << temp.getPassword() << setw(6) << ((temp.getAdminMode() == true) ? "Да" : "Нет") << endl;
	}
	cout.fill('-');
	cout << setw(49) << '-' << endl;
	cout.fill(' ');
}

void SClient::addUser()
{
	sendToS("Start add users");
	while (true)
	{
		system("cls");
		system("title Добавление пользователя");
		string temp;
		cout << "Добавление нового пользователя\n" << endl;
		cout << "Введите фамилию: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "Введите имя: ";
		rewind(stdin);
		cin >> temp;
		sendToS(temp);
		cout << "Введите логин: ";
		rewind(stdin);
		getline(cin, temp);
		sendToS(temp);
		cout << "Введите пароль: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "Выберите роль пользователя\n1. Администратор\n0. Пользователь (эксперт)" << endl;
		sendToS(to_string(getMenuChoice(2)));
		serverMsg = recvFromS();
		if (serverMsg == "Successfully added")
			cout << "Пользователь успешно добавлен!" << endl;
		else
			if (serverMsg == "Already exist")
				cout << "Пользователь с таким логином и паролем уже существует!" << endl;
		cout << "Закончить работу?\n1. Да\n0. Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

void SClient::editUser() // надо разбить меню редактирования на части, а то нельзя изменить только фамилию и имя, без изменения логина или пароля
{
	sendToS("Start edit users");
	while (true)
	{
		printUsers();
		cout << "Выберите пользователя для редактирования" << endl;
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "Вы не можете изменять активного пользователя!" << endl;
		else
		{

			string temp;
			cout << "Введите новые данные" << endl;
			cout << "Введите фамилию: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите имя: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите логин: ";
			rewind(stdin);
			getline(cin, temp);
			sendToS(temp);
			cout << "Введите пароль: ";
			rewind(stdin);
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "Выберите роль пользователя\n1. Администратор\n0. Пользователь (эксперт)" << endl;
			sendToS(to_string(getMenuChoice(2)));
			serverMsg = recvFromS();
			if (serverMsg == "Already exist")
				cout << "Пользователь с таким логином и паролем уже существует!" << endl;
		}
		cout << "Закончить работу?\n1. Да\n0. Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

void SClient::deleteUser()
{
	sendToS("Start delete users");
	while (true)
	{
		printUsers();
		cout << "Выберите пользователя для удаления: ";
		int num = stoi(recvFromS());
		sendToS(to_string(getMenuChoice(num, 1)));
		
		serverMsg = recvFromS();
		if (serverMsg == "Active user")
			cout << "Вы не можете удалить активного пользователя!" << endl;
		if (serverMsg == "Successfully deleted")
			cout << "Пользователь успешно удалён!" << endl;
		cout << "Закончить работу?\n1. Да\n0. Нет" << endl;
		if (getMenuChoice(2))
			break;
	}
	sendToS("Back");
}

void SClient::infoGroups()
{

	sendToS("Start print groups");
	while (true) {
		system("cls");
		system("title Оценочные группы");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найдена ни одна группа!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер группы для того чтобы просмотреть подробную информацию\n" << endl;
			cout << setw(3) << "#" << setw(20) << "Название группы" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0. Назад" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			if (recvFromS() == "Report ready")
			{
				cout << "Для данной группы готов отчёт! Желаете его просмотреть?\n1. Да\n0. Нет\n";
				choice = getMenuChoice(2);
				if (choice)
				{
					sendToS("Print report");
					system("cls");
					cout << recvFromS();
				}
				else
					sendToS("Dont print");
			}
			if (choice)
			system("pause");
		}
	}
	sendToS("Back");
}
void SClient::addProdGroup() // создавать копию объекта и в самом конце обновлять только если она есть в базе
{
	sendToS("Start add group");
	system("cls");
	system("title Добавление оценочной группы");
	string temp;
	User tempU;
	bool success = false;
	serverMsg = recvFromS();
	if (serverMsg == "Not ready to add")
	{
		cout << "Ошибка! В базе должно быть больше одного продукта и хотя бы 1 эксперт для оценки." << endl;
		system("pause");
	}
	else
	{
		cout << "Введите название группы: ";
		rewind(stdin);
		getline(cin, temp);
		rewind(stdin);
		sendToS(temp);
		cout << "Введите маскимум оценочной шкалы: ";
		sendToS(to_string(getNum<int>()));
		while (true)
		{
			system("cls");
			cout << "Выберите товары для оценки" << endl;
			cout << setw(3) << "#" << " Название и id" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS();
				cout << " (id " << recvFromS() << ")" << endl;
			}
			cout << "\n0. Продолжить" << endl;
			choice = getMenuChoice(count+1);
			if (!choice)
			{
				sendToS("Next stage");
				break;
			}
			else
				sendToS(to_string(choice - 1));
		}
		while (true)
		{
			system("cls");
			cout << "Выберите ответственных за оценку" << endl;
			cout << setw(3) << "#" << " Фамилия Имя (логин)" << endl;
			int count = stoi(recvFromS()), addedCount = 0, choice;
			for (int i = 0; i < count; i++)
			{
				cout << setw(3) << i+1 << " " << recvFromS() << endl;
			}
			cout << "\n0. Продолжить" << endl;
			choice = getMenuChoice(count + 1);
			if (!choice)
			{
				sendToS("Next stage");
				break;
			}
			else
				sendToS(to_string(choice - 1));
		}
		cout << recvFromS() << endl;
		system("pause");
	}
	sendToS("Back");
}
void SClient::delProdGroup() // удалять группу у всех пользователей
{
	sendToS("Start delete group");
	while (true) {
		system("cls");
		system("title Оценочные группы");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найдена ни одна группа!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер группы для удаления" << endl;
			cout << setw(3) << "#" << setw(20) << "Название группы" << setw(15) << "Статус отчёта" << endl;
			cout.fill('-');
			cout << setw(38) << '-' << endl;
			cout.fill(' ');

			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(20) << recvFromS();
				cout << setw(15) << recvFromS() << endl;
			}
			cout << "\n0. Выход" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS() << endl;
			system("pause");
		}
	}
	sendToS("Back");
}
void SClient::printProducts()
{
	sendToS("Start print products");
	while (true)
	{
		system("cls");
		system("title Список товаров");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найден ни один товар!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер товара для того чтобы просмотреть подробную информацию\n" << endl;
			cout << setw(3) << "#" << setw(40) << "Название товара" << setw(10) << "ID" << endl;
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(40) << recvFromS();
				cout << setw(10) << recvFromS() << endl;
			}
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			cout << "0. Назад" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS();
			system("pause");
		}

	}
	sendToS("Back");
}
void SClient::addProduct()
{
	string temp;
	sendToS("Start add product");
	while (true)
	{
		system("cls");
		system("title Добавление продукта");
		cout << "Заполните данные о продукте\nID: ";
		temp = to_string(getNum<int>());
		rewind(stdin);
		sendToS(temp);
		if (recvFromS() == "Already exist")
		{
			cout << "Продукт с таким ID уже существует! Выберите другой ID" << endl;
			system("pause");
		}
		else
		{
			cout << "Название: ";
			getline(cin, temp);
			rewind(stdin);
			sendToS(temp);
			cout << "Цена: ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			cout << "Стоимость хранения: ";
			temp = to_string(getNum <double>());
			sendToS(temp);
			cout << "Срок реализации (дней до списания): ";
			temp = to_string(getNum<int>());
			sendToS(temp);
			cout << "Процент списаний (без \'%\'): ";
			temp = to_string(getNum<double>());
			sendToS(temp);
			serverMsg = recvFromS();
			if (serverMsg == "Successfully added")
				cout << "Продукт успешно добавлен!" << endl;
			cout << "Завершить работу?\n1. Да\n0. Нет, добавить ещё\n";
			if (getMenuChoice(2))
				break;
		}
	}
	sendToS("Back");
}
void SClient::delProduct()
{
	sendToS("Start delete product");

	while (true)
	{
		system("cls");
		system("title Список товаров");
		int count = stoi(recvFromS()), choice;
		if (count == 0)
		{
			cout << "Не найден ни один товар!" << endl;
			choice = 0;
			system("pause");
		}
		else
		{
			cout << "Выберите номер товара для удаления" << endl;
			cout << setw(3) << "#" << setw(40) << "Название товара" << setw(10) << "ID" << endl;
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			for (int i = 1; i <= count; i++)
			{
				cout << setw(3) << i << setw(40) << recvFromS();
				cout << setw(10) << recvFromS() << endl;
			}
			cout.fill('-');
			cout << setw(53) << '-' << endl;
			cout.fill(' ');
			cout << "0. Назад" << endl;
			choice = getMenuChoice(count + 1);
		}
		if (!choice)
			break;
		else
		{
			system("cls");
			sendToS(to_string(choice - 1));
			cout << recvFromS() << endl;
			system("pause");
		}

	}

	sendToS("Back");
}
void SClient::saveData()
{
	sendToS("Save all data");
}
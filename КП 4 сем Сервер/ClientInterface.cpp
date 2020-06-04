#include "ClientInterface.h"


ClientInterface::ClientInterface(SOCKET s)
{
	client_socket = s;
}

string ClientInterface::commaToDot(string s)
{
	if (s.find(',') != string::npos)
		s[s.find(',')] = '.';
	return s;
}

ClientInterface::~ClientInterface()
{
}

void ClientInterface::sendToClient(string str)
{
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	for (unsigned int i = 0; i < (str.length() / (BUFFER_SIZE - 1)); i++)
	{
		buf[0] = '1';
		str.copy(buf + 1, BUFFER_SIZE - 1, i * (BUFFER_SIZE - 1));
		if (send(client_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD_EX("Клиент оборвал соединение");
	}
	buf[0] = '0';
	str.copy(buf + 1, BUFFER_SIZE - 1, str.length() / (BUFFER_SIZE - 1));
	buf[str.length() % (BUFFER_SIZE - 1) + 1] = '\0';
	if (send(client_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD_EX("Клиент оборвал соединение");
}

string ClientInterface::recvFromClient(bool hideLog)
{
	string result = "";
	const int BUFFER_SIZE = 1024;
	char buf[BUFFER_SIZE];
	if (recv(client_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		throw BREAK_THREAD_EX("Клиент оборвал соединение");
	
	result += buf + 1;
	while (buf[0] == 1)
	{
		if (recv(client_socket, buf, sizeof(buf), 0) == SOCKET_ERROR)
			throw BREAK_THREAD_EX("Клиент оборвал соединение");
		result += buf + 1;
	}
	cout << "Поток " <<setw(5) <<  this_thread::get_id() << " : получена строка ";// для отчётов в консоль сервера
	if (!hideLog)
		cout << '\"' << result << '\"' << endl;
	else
		cout << "#СКРЫТО" << endl;
	return result;
}

void ClientInterface::startMenu()
{
	while (true) {
		currentCommand = recvFromClient();
		if (currentCommand == "Exit")
			break;

		if (currentCommand == "Start authorization")
			auth();
		if (currentCommand == "Start registration")
			registration();
	}
}

void ClientInterface::auth()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start admin menu")
			adminMenu();
		else
			if (currentCommand == "Start user menu")
				userMenu();
			else
			{
				User tempUser;
				tempUser.setLogin(currentCommand);
				currentCommand = recvFromClient(true);
				tempUser.setPassword(currentCommand);
				opDataBase::Hinstance().userVector;
				vector<User>::iterator iter;
				iter = find(opDataBase::Hinstance().userVector.begin(), opDataBase::Hinstance().userVector.end(), tempUser);
				if (iter != opDataBase::Hinstance().userVector.end())
				{
					sendToClient("Founded");
					sendToClient(to_string(iter->getAdminMode()));
					sendToClient(iter->getFName());
					sendToClient(iter->getLName());
					opDataBase::Hinstance().activeUsers.push_back(*iter);
					currentUser = *iter;
					currentCommand = recvFromClient();
					if (currentCommand == "Start admin menu")
						adminMenu();
					if (currentCommand == "Start user menu")
						userMenu();
				}
				else
					sendToClient("Not founded");
			}
	}
}

void ClientInterface::registration()
{
	while (true)
	{
		User temp;
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		temp.setLName(currentCommand);
		temp.setFName(recvFromClient());
		temp.setLogin(recvFromClient());
		temp.setPassword(recvFromClient(1));
		vector<User>::iterator iter;
		iter = find(opDataBase::Hinstance().userVector.begin(), opDataBase::Hinstance().userVector.end(), temp);
		if (iter != opDataBase::Hinstance().userVector.end())
			sendToClient("Already exist");
		else
		{
			opDataBase::Hinstance().userVector.push_back(temp);
			sendToClient("Successful registration");
		}
	}
}

void ClientInterface::adminMenu()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start user control menu")
			userControlMenu();
		if (currentCommand == "Start products control menu")
			productsControlMenu();
		if (currentCommand == "Start print full report")
			printFullReport();
		if (currentCommand == "Save all data")
		{
			cout << "Клиент из потока : " << this_thread::get_id() << " вызвал сохранение данных" << endl;
			opDataBase::Hinstance().saveAll();
		}
	}
}

void ClientInterface::userMenu()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start expert rate menu")
			expertRateMenu();
		if (currentCommand == "Start print products")
			printProducts();
		if (currentCommand == "Start print groups")
			printGroups();
		if (currentCommand == "Start print full report")
			printFullReport();

	}
}

void ClientInterface::expertRateMenu()
{
	while (true)
	{
		vector <ProductGroup> prodGrPull;
		int count = 0;
		for (auto &iter : opDataBase::Hinstance().prodGroupVector)
		{
			if (iter.experts.find( currentUser) != iter.experts.end())
				if(iter.experts[currentUser].size() == 0)
					prodGrPull.push_back(iter);
		}
		sendToClient(to_string(prodGrPull.size())); //количество групп, в которых участвует юзер
		string result = "";
		for (auto &iter : prodGrPull) //вывод для этих групп
		{
			sendToClient(iter.getName());
			sendToClient(iter.isReady() ? "Готов" : "Не готов");
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		else
		{
			int num = stoi(currentCommand);
			vector<ProductGroup>::iterator iter;
			iter = prodGrPull.begin() + num;
			result += "Название группы товаров: " + iter->getName() + "\n\nОтветственные за экспертные оценки\n" + iter->getExpertsForPrint() + "Перечень товаров подлежащих оценке:\n" + iter->getProductsForPrint();
			sendToClient(result);
			currentCommand = recvFromClient();
 			if (currentCommand == "Start rate")
			{
				ProductGroup prGr =prodGrPull[num];
				int **matrix = new int*[prGr.products.size()];
				for (unsigned int i = 0; i < prGr.products.size(); i++)
					matrix[i] = new int[prGr.products.size()];
				int i = 0, j;
				sendToClient(to_string(prGr.getMaxOfScale()));
				sendToClient(to_string(prGr.products.size()));
				map<Product, double>::iterator iter1;
				for (iter1 = prGr.products.begin(); iter1 != prGr.products.end(); iter1++)
				{
					j = i + 1;
					iter1++;
					map<Product, double>::iterator iter2 = iter1;
					iter1--;
					for (; iter2 != prGr.products.end(); iter2++)
					{
						sendToClient("Товар 1\n"+iter1->first.getFullInfo());
						sendToClient("\nТовар 2\n"+iter2->first.getFullInfo());
						sendToClient("\nВ скольких случаях из " + to_string(prGr.getMaxOfScale()) + " товар 1 будет предпочтительнее, чем товар 2?");
						int mark = stoi(recvFromClient());
						matrix[i][j] = mark;
						matrix[j][i] = prGr.getMaxOfScale() - mark;
						j++;
					}
					i++;
				}
				for (unsigned int i = 0; i < prGr.products.size(); i++)
				{
					double sum = 0;
					for (unsigned int j = 0; j < prGr.products.size(); j++)
					{
						if (i != j)
						{
							sum += matrix[i][j];
						}
					}
					prGr.experts[currentUser].push_back(sum / prGr.getMaxOfScale());
				}
				//продолжить чи не 
				prGr.checkReady();
				(*find(opDataBase::Hinstance().prodGroupVector.begin(), opDataBase::Hinstance().prodGroupVector.end(), prodGrPull[num])) = prGr;
				sendToClient("Оценки успешно выставлены!");	
			}
			else
				if (currentCommand == "Back")
					break;
		}
	}
}

void ClientInterface::userControlMenu()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start print users")
			printUsers();
		if (currentCommand == "Start add users")
			addUser();
		if (currentCommand == "Start edit users")
			editUser();
		if (currentCommand == "Start delete users")
			deleteUser();
	}
}

void ClientInterface::printUsers()
{
	sendToClient(to_string(opDataBase::Hinstance().userVector.size()));
	for (auto &iter : opDataBase::Hinstance().userVector)
	{
		sendToClient(iter.getFName());
		sendToClient(iter.getLName());
		sendToClient(iter.getLogin());
		sendToClient(iter.getPassword());
		sendToClient(to_string(iter.getAdminMode()));
	}
}

void ClientInterface::addUser()
{

	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		User temp;
		temp.setLName(currentCommand);
		temp.setFName(recvFromClient());
		temp.setLogin(recvFromClient());
		temp.setPassword(recvFromClient(1));
		temp.setAdminMode(stoi(recvFromClient()));
		vector<User>::iterator iter;
		iter = find(opDataBase::Hinstance().userVector.begin(), opDataBase::Hinstance().userVector.end(), temp);
		if (iter != opDataBase::Hinstance().userVector.end())
			sendToClient("Already exist");
		else
		{
			opDataBase::Hinstance().userVector.push_back(temp);
			sendToClient("Successfully added");
		}
	}
}

void ClientInterface::editUser()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start print users")
			printUsers();
		sendToClient(to_string(opDataBase::Hinstance().userVector.size()));
		currentCommand = recvFromClient();
		int num = stoi(currentCommand);
		vector<User>::iterator iter;
		iter = find(opDataBase::Hinstance().activeUsers.begin(), opDataBase::Hinstance().activeUsers.end(), opDataBase::Hinstance().userVector[num - 1]);
		if ((iter != opDataBase::Hinstance().activeUsers.end())/* && !(*iter == currentUser)*/)
			sendToClient("Active user");
		else
		{
			sendToClient("Start fill info");
			User temp;
			temp.setLName(recvFromClient());
			temp.setFName(recvFromClient());
			temp.setLogin(recvFromClient());
			temp.setPassword(recvFromClient(1));
			temp.setAdminMode(stoi(recvFromClient()));


			iter = find(opDataBase::Hinstance().userVector.begin(), opDataBase::Hinstance().userVector.end(), temp);
			if (iter != opDataBase::Hinstance().userVector.end())
				sendToClient("Already exist");
			else
			{
				opDataBase::Hinstance().userVector[num - 1] = temp;
				sendToClient("Successfully edited");
			}
		}
	}
}

void ClientInterface::deleteUser()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start print users")
			printUsers();
		sendToClient(to_string(opDataBase::Hinstance().userVector.size()));
		int num = stoi(recvFromClient());
		vector<User>::iterator iter;
		iter = find(opDataBase::Hinstance().activeUsers.begin(), opDataBase::Hinstance().activeUsers.end(), opDataBase::Hinstance().userVector[num-1]);
		if (iter != opDataBase::Hinstance().activeUsers.end() && !(*iter == currentUser))
			sendToClient("Active user");
		else
		{
			opDataBase::Hinstance().userVector.erase(opDataBase::Hinstance().userVector.begin() + num - 1);
			sendToClient("Successfully deleted");
		}
	}
}

void ClientInterface::productsControlMenu()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		if (currentCommand == "Start print groups")
			printGroups();
		if (currentCommand == "Start add group")
			addProdGroup();
		if (currentCommand == "Start delete group")
			delProdGroup();
		if (currentCommand == "Start print products")
			printProducts();
		if (currentCommand == "Start add product")
			addProduct();
		if (currentCommand == "Start delete product")
			delProduct();
	}
}

void ClientInterface::printGroups()
{
	while (true)
	{	
		sendToClient(to_string(opDataBase::Hinstance().prodGroupVector.size()));
		string result = "";
		for (auto &iter : opDataBase::Hinstance().prodGroupVector)
		{
			sendToClient(iter.getName());
			sendToClient(iter.isReady() ? "Готов" : "Не готов");
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		else
		{
			vector<ProductGroup>::iterator iter;
			iter = opDataBase::Hinstance().prodGroupVector.begin() + (stoi(currentCommand));
			result+= "Название группы товаров: " + iter->getName()+ "\n\nОтветственные за экспертные оценки\n" + iter->getExpertsForPrint() + "Перечень товаров подлежащих оценке:\n" + iter->getProductsForPrint();
			sendToClient(result);
			iter->checkReady();
			if (iter->isReady())
			{
				sendToClient("Report ready");
				if (recvFromClient() == "Print report")
					sendToClient(iter->getReport());
			}
			else
				sendToClient("Not ready");
		}
	}
}
void ClientInterface::addProdGroup()
{
	while (true)
	{
		int countOfExp = 0;
		for (auto &iter : opDataBase::Hinstance().userVector)
			countOfExp += !iter.getAdminMode();
		if (countOfExp == 0 || (opDataBase::Hinstance().productVector.size() < 2))
			sendToClient("Not ready to add");
		else
		{
			sendToClient("OK");
			string temp;
			int tempSc;
			temp = recvFromClient();
			tempSc = stoi(recvFromClient());
			vector <User> tempVUPull;
			vector <User> tempVU;
			vector <Product> tempVPPull = opDataBase::Hinstance().productVector;
			vector <Product> tempVP;
			for (auto &iter : opDataBase::Hinstance().userVector)
				if (!iter.getAdminMode())
				{
					tempVUPull.push_back(iter);
				}
			while (true)
			{
				sendToClient(to_string(tempVPPull.size()));
				for (auto &iter : tempVPPull)
				{
					sendToClient(iter.getName());
					sendToClient(to_string(iter.getID()));
				}
				currentCommand = recvFromClient();
				if (currentCommand == "Next stage")
					break;
				tempVP.push_back(tempVPPull[stoi(currentCommand)]);
				tempVPPull.erase(tempVPPull.begin() + stoi(currentCommand));
			}
			while (true)
			{
				sendToClient(to_string(tempVUPull.size()));
				for (auto &iter : tempVUPull)
				{
					sendToClient(iter.getLName() + " " + iter.getFName() + " (" + iter.getLogin() + ")");
				}
				currentCommand = recvFromClient();
				if (currentCommand == "Next stage")
					break;
				tempVU.push_back(tempVUPull[stoi(currentCommand)]);
				tempVUPull.erase(tempVUPull.begin() + stoi(currentCommand));
			}
			opDataBase::Hinstance().prodGroupVector.push_back(ProductGroup(temp, tempSc, tempVU, tempVP));
			opDataBase::Hinstance().saveProductGroups(); //!!!!!
			sendToClient("Группа успешно добавлена!");
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
	}
}
void ClientInterface::delProdGroup()
{
	while (true)
	{
		sendToClient(to_string(opDataBase::Hinstance().prodGroupVector.size()));
		string result = "";
		for (auto &iter : opDataBase::Hinstance().prodGroupVector)
		{
			sendToClient(iter.getName());
			sendToClient(iter.isReady() ? "Готов" : "Не готов");
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		else
		{
			vector<ProductGroup>::iterator iter;
			opDataBase::Hinstance().prodGroupVector.erase( opDataBase::Hinstance().prodGroupVector.begin() + (stoi(currentCommand)));
			result = "Группа товаров успешно удалена";
			sendToClient(result);
		}
	}
}
void ClientInterface::printProducts()
{
	while (true)
	{
		sendToClient(to_string(opDataBase::Hinstance().productVector.size()));
		string result = "";
		sort(opDataBase::Hinstance().productVector.begin(), opDataBase::Hinstance().productVector.begin());
		for (auto &iter : opDataBase::Hinstance().productVector)
		{
			sendToClient(iter.getName());
			sendToClient(to_string(iter.getID()));
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		else
		{
			vector<Product>::iterator iter;
			iter = opDataBase::Hinstance().productVector.begin() + (stoi(currentCommand));
			sendToClient(iter->getFullInfo());
		}

	}
}
void ClientInterface::addProduct()
{
	while (true)
	{
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		Product temp(stoi(currentCommand));
		if (find(opDataBase::Hinstance().productVector.begin(), opDataBase::Hinstance().productVector.end(), temp) == opDataBase::Hinstance().productVector.end())
		{
			sendToClient("OK");
			temp.setName(recvFromClient());
			setlocale(LC_ALL, "C");
			temp.setPrice(stod(commaToDot(recvFromClient())));
			temp.setStorageCost(stod(commaToDot(recvFromClient())));
			temp.setSellByDays(stoi(recvFromClient()));
			temp.setNonRealizedPercent(stod(commaToDot( recvFromClient())));
			setlocale(LC_ALL, "rus");
			opDataBase::Hinstance().productVector.push_back(temp);
			sendToClient("Successfully added");
		}
		else
			sendToClient("Already exist");
	}
}

void ClientInterface::delProduct()
{
	while (true)
	{
		sendToClient(to_string(opDataBase::Hinstance().productVector.size()));
		string result = "";
		for (auto &iter : opDataBase::Hinstance().productVector)
		{
			sendToClient(iter.getName());
			sendToClient(to_string(iter.getID()));
		}
		currentCommand = recvFromClient();
		if (currentCommand == "Back")
			break;
		else
		{
			vector<ProductGroup>::iterator iter;
			opDataBase::Hinstance().productVector.erase(opDataBase::Hinstance().productVector.begin()+stoi(currentCommand));
			result = "Товар успешно удалён!";
			sendToClient(result);
		}
	}
}

void ClientInterface::printFullReport()
{
	for (auto &iter : opDataBase::Hinstance().prodGroupVector)
	{
		if (iter.isReady())
			sendToClient(iter.getReport());
	}
	sendToClient("End");
	for (auto &iter : opDataBase::Hinstance().productVector)
	{
		sendToClient(iter.getFullInfo());
	}
	sendToClient("End");
}
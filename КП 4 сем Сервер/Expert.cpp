#include "Expert.h"



Expert::Expert()
{
}


Expert::~Expert()
{
}
bool Expert::operator==(User u)
{
	return (this->getLogin() == u.getLogin() && this->getPassword() == u.getPassword());
}
void Expert::loadUncheckedGr() //загрузка групп, требующих оценки
{

}


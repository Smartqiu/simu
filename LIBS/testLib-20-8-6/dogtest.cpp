//派生类DogTest的实现文件dogtest.cpp
/////////////////////////////////////////////////////////////////////////
#include "dogtest.h"
#include "canimal.h"
#include <QMessageBox>

CAnimal* CreateDog()
{
    return DogTest::instance();
}

void ReleaseDog()
{
    if (DogTest::instance() != NULL)
        delete DogTest::instance();
}

DogTest::DogTest()
    : CAnimal()
{
}

DogTest* DogTest::s_instance = NULL;

DogTest* DogTest::instance()
{
    if (NULL == s_instance)
        s_instance = new DogTest;
    return s_instance;
}

void DogTest::setSysCtrl(CSysCtrl *sysctrl)
{
    m_sysctrl = sysctrl;
}

bool DogTest::eat()
{
    afs->CreateRecv();
    QMessageBox::warning(NULL, "warning", "Haha!Dog is eating food !");
    return true;
}

void DogTest::sleep()
{
    afs->sendMsg(QString("127.0.0.1"),QString("21778"),QString("你好，世界"));
    QMessageBox::warning(NULL, "warning", "Be quit!Dog is sleep now !");
}
/////////////////////////////////////////////////////////////////////////

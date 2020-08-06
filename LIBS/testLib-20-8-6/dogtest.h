#ifndef DOGTEST_H
#define DOGTEST_H

#include "canimal.h"

#define afs DogTest::instance()->GetSysCtrl()

//创建DogTest对象
extern "C" Q_DECL_EXPORT CAnimal* CreateDog();
extern "C" Q_DECL_EXPORT void ReleaseDog();

class DogTest : public CAnimal
{
public:
    bool eat();
    void sleep();
    void setSysCtrl(CSysCtrl* sysctrl);

    static DogTest* instance();

    CSysCtrl* GetSysCtrl(){return m_sysctrl;}

protected:
    DogTest();

private:
    static DogTest* s_instance;
};

#endif // DOGTEST_H
/////////////////////////////////////////////////////////////////////////

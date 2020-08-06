#ifndef MODULESINTERFACE_H
#define MODULESINTERFACE_H

#include <QObject>
#include "declare.h"
#include "frameinterface.h"

//dll每个模块需要实现的接口
//只有实现了这个接口才能被加载到主框架中
class AFS_SHARED_EXPORT IModules
{
public:
    virtual QIcon* GetModuleIcon() = 0;     //获取图标
    virtual QString* GetModuleText() = 0;   //获取文本
    virtual void setSysCtrl(IFrame* framectrl) = 0; //传递主框架接口实例，重要
protected:
    IFrame* m_framectrl;
};

#endif // MODULESINTERFACE_H

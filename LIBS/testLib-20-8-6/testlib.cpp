#include "testlib.h"
#include "form.h"
Form *f = NULL;
QWidget* Create(QWidget* widget)
{
    if(!f)
    {
        f = new Form(widget);
    }
    return f;
}

QIcon* GetModuleIcon()
{
    QIcon *icon = new QIcon(":/set2.png");   return icon;
}
QString* GetModuleText()
{
    return new QString("启动控制");
}

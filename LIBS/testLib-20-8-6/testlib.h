#ifndef TESTLIB_H
#define TESTLIB_H

#include "testLib_global.h"
#include <QWidget>
#include <QIcon>
extern "C" {
    TESTLIB_EXPORT QWidget* Create(QWidget* widget);
    TESTLIB_EXPORT QIcon* GetModuleIcon();
    TESTLIB_EXPORT QString* GetModuleText();
}

#endif // TESTLIB_H

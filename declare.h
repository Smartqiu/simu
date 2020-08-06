#ifndef DECLARE_H
#define DECLARE_H

#include <QtCore/qglobal.h>

#if defined(AFS_FRAMEWORK)
#  define AFS_SHARED_IMPORT Q_DECL_EXPORT
#  define AFS_SHARED_EXPORT Q_DECL_IMPORT
#else
#  define AFS_SHARED_IMPORT Q_DECL_IMPORT
#  define AFS_SHARED_EXPORT Q_DECL_EXPORT
#endif

#endif // DECLARE_H

#ifndef NUTMEGLIBDECL_H
#define NUTMEGLIBDECL_H

#include <QtCore/qglobal.h>

#if defined NUTMEG_LIBRARY
 #define NUTMEGLIB_EXPORT Q_DECL_EXPORT
#else
 #define NUTMEGLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // NUTMEGLIBDECL_H

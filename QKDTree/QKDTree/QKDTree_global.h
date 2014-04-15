#ifndef QKDTREE_GLOBAL_H
#define QKDTREE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QKDTREE_LIBRARY)
#  define QKDTREESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QKDTREESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QKDTREE_GLOBAL_H

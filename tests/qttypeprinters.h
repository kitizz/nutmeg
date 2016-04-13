#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <gtest/gtest.h>

#include <QString>

#include <ostream>

QT_BEGIN_NAMESPACE
inline void PrintTo(const QString &qString, ::std::ostream *os)
{
    *os << qPrintable(qString);
}
QT_END_NAMESPACE

#endif // TESTHELPERS_H

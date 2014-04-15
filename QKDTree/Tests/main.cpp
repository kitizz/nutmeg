#include <QtTest>
#include "tst_QKDTreeTests.h"

int main(int, char**)
{
    QKDTreeTests tests;
    QTest::qExec(&tests);

    return 0;
}

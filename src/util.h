#ifndef UTIL_H
#define UTIL_H

#include <QList>

class Util
{
public:
    Util();

    // TODO: The following functions should find their way to a static util library...
    static QList<int>* medianArray(int N, int s=0, QList<int> *lst=0);
    static int randInt(int low, int high);
    static QList<int> *randomArray(int N);
};

#endif // UTIL_H

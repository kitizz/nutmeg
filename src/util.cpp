#include "util.h"

Util::Util()
{
}

int Util::randInt(int low, int high) {
    // From: http://goo.gl/KdHeUg
    return qrand() % ((high + 1) - low) + low;
}

QList<int>* Util::randomArray(int N)
{
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    QList<int> *lst = new QList<int>();
    lst->reserve(N);

    for (int i=0; i<N; ++i)
        (*lst)[i] = i;

    for (int i=0; i<N; ++i) {
        int n = randInt(i, N-1);
        lst->swap(i, n);
    }

    return lst;
}

QList<int>* Util::medianArray(int N, int s, QList<int>* lst)
{
    if (!lst) {
        lst = new QList<int>();
        lst->reserve(N);
    }

    if (N - s <= 2) {
        lst->append(s);
        if (N - s == 2)
            lst->append(s + 1);
        return lst;
    }

    int i = (s + N - 1)/2;
    lst->append(i);

    medianArray(i, s, lst);
    medianArray(N, i + 1, lst);

    return lst;
}

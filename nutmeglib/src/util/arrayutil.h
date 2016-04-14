#ifndef ARRAYUTIL_H
#define ARRAYUTIL_H

#include <QObject>
#include "ndarray.h"

#define EXPAND_METHOD(FNAME, T, DEFAULT)\
static T FNAME (const NDArray &array) {\
    switch (array.type()) {\
    case NDArray::Int64:\
        return FNAME (array.convert<int>());\
    case NDArray::Float32:\
        return FNAME (array.convert<float>());\
    case NDArray::Float64:\
        return FNAME (array.convert<double>());\
    case NDArray::Uint8:\
        return FNAME (array.convert<uint8_t>());\
    default:\
        return DEFAULT;\
    }\
}

class Range {
public:
    Range(qreal min, qreal max) : min(min), max(max) {}
    qreal min;
    qreal max;
};

class ArrayUtil : public QObject
{
    Q_OBJECT
protected:
    template <typename T>
    static qreal min(const NDArrayTyped<T> &array)
    {
        auto it = array.cbegin();
        auto end = array.cend();
        T val = *it;
        while (++it != end)
            val = qMin(val, *it);

        return val;
    }

    template <typename T>
    static qreal max(const NDArrayTyped<T> &array)
    {
        auto it = array.cbegin();
        auto end = array.cend();
        T val = *it;
        while (++it != end)
            val = qMax(val, *it);

        return val;
    }

    template <typename T>
    static Range limits(const NDArrayTyped<T> &array)
    {
        auto it = array.cbegin();
        auto end = array.cend();
        T min_val = *it;
        T max_val = *it;
        while (++it != end) {
            min_val = qMin(min_val, *it);
            max_val = qMax(max_val, *it);
        }

        return Range(min_val, max_val);
    }

public:
    explicit ArrayUtil(QObject *parent = 0);

    EXPAND_METHOD(min, qreal, 0)
    EXPAND_METHOD(max, qreal, 0)
    EXPAND_METHOD(limits, Range, Range(0,0))

signals:

public slots:

};

#endif // ARRAYUTIL_H

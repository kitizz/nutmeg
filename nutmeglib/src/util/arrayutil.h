#ifndef ARRAYUTIL_H
#define ARRAYUTIL_H

#include <QObject>
#include <QList>
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
    case NDArray::Bool:\
        return FNAME (array.convert<bool>());\
    default:\
        return DEFAULT;\
    }\
}

#define EXPAND_METHOD1(FNAME, T, DEFAULT, TYPE1)\
static T FNAME (const NDArray &array, TYPE1 arg1) {\
    switch (array.type()) {\
    case NDArray::Int64:\
        return FNAME (array.convert<int>(), arg1);\
    case NDArray::Float32:\
        return FNAME (array.convert<float>(), arg1);\
    case NDArray::Float64:\
        return FNAME (array.convert<double>(), arg1);\
    case NDArray::Uint8:\
        return FNAME (array.convert<uint8_t>(), arg1);\
    case NDArray::Bool:\
        return FNAME (array.convert<bool>(), arg1);\
    default:\
        return DEFAULT;\
    }\
}

#define EXPAND_METHOD2(FNAME, T, DEFAULT, TYPE1, TYPE2)\
static T FNAME (const NDArray &array, TYPE1 arg1, TYPE2 arg2) {\
    switch (array.type()) {\
    case NDArray::Int64:\
        return FNAME (array.convert<int>(), arg1, arg2);\
    case NDArray::Float32:\
        return FNAME (array.convert<float>(), arg1, arg2);\
    case NDArray::Float64:\
        return FNAME (array.convert<double>(), arg1, arg2);\
    case NDArray::Uint8:\
        return FNAME (array.convert<uint8_t>(), arg1, arg2);\
    case NDArray::Bool:\
        return FNAME (array.convert<bool>(), arg1, arg2);\
    default:\
        return DEFAULT;\
    }\
}

class NUTMEGLIB_EXPORT RangeValues {
public:
    RangeValues(qreal min, qreal max) : min(min), max(max) {}
    qreal min;
    qreal max;
};

class ArrayUtil : public QObject
{
    Q_OBJECT
public:
    template <typename T>
    static qreal min(const NDArrayTyped<T> &array)
    {
        if (array.size() == 0) {
            qWarning() << "WARNING: Finding min of empty array!";
            return 0;
        }
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
        if (array.size() == 0) {
            qWarning() << "WARNING: Finding max of empty array!";
            return 0;
        }
        auto it = array.cbegin();
        auto end = array.cend();
        T val = *it;
        while (++it != end)
            val = qMax(val, *it);

        return val;
    }

    template <typename T>
    static RangeValues limits(const NDArrayTyped<T> &array)
    {
        if (array.size() == 0) {
            qWarning() << "WARNING: Finding limits of empty array!";
            return RangeValues(0,0);
        }
        auto it = array.cbegin();
        auto end = array.cend();
        T min_val = *it;
        T max_val = *it;
        while (++it != end) {
            min_val = qMin(min_val, *it);
            max_val = qMax(max_val, *it);
        }

        return RangeValues(min_val, max_val);
    }

    template <typename T>
    static QList<RangeValues> limitsAxis(const NDArrayTyped<T> &array, int axis)
    {
        QList<RangeValues> res;
        if (array.ndim() != 2) {
            qWarning() << "WARNING: Finding limits of non 2D array!";
            return res;
        }

        int axis2 = (axis + 1) % 2;
        int N = array.shape(axis);
        int M = array.shape(axis2);
        for (int i=0; i<M; ++i)
            res << RangeValues(0,0);

        if (array.size() == 0) {
            qWarning() << "WARNING: Finding limits of empty array!";
            return res;
        }

        if (axis == 1) {
            // Can do each row as a chunk
            for (int i=0; i<M; ++i) {
                auto it = array.cbegin() + i*N;
                auto end = it + N;
                T min_val = *it;
                T max_val = *it;
                while (++it != end) {
                    min_val = qMin(min_val, *it);
                    max_val = qMax(max_val, *it);
                }
                res[i].min = min_val;
                res[i].max = max_val;
            }

        } else if (axis == 0) {
            // Need to stride for each column
            for (int i=0; i<M; ++i) {
                auto it = array.cbegin() + i;
                T min_val = *it;
                T max_val = *it;
                int n = N;
                while (n--) {
                    min_val = qMin(min_val, *it);
                    max_val = qMax(max_val, *it);
                    it += M;
                }
                res[i].min = min_val;
                res[i].max = max_val;
            }
        }

        return res;
    }

    template <typename T>
    static RangeValues limitsWrap(const NDArrayTyped<T> &array, int start, int N)
    {
        if (array.size() == 0) {
            qWarning() << "WARNING: Finding limits of empty array!";
            return RangeValues(0,0);
        }
        auto it = array.cbegin() + start;
        int M = array.size();
        start = start % M;

        T min_val = *it;
        T max_val = *it;

        // First part
        int i = qMin(N, M - start);
        while (i--) {
            min_val = qMin(min_val, *it);
            max_val = qMax(max_val, *it);
            ++it;
        }
        it = array.cbegin();
        // Next part
        i = qMin(N - (M - start), start);
        if (i > 0) {
            while (i--) {
                min_val = qMin(min_val, *it);
                max_val = qMax(max_val, *it);
                ++it;
            }
        }

        return RangeValues(min_val, max_val);
    }

//public:
    explicit ArrayUtil(QObject *parent = 0);

    EXPAND_METHOD(min, qreal, 0)
    EXPAND_METHOD(max, qreal, 0)
    EXPAND_METHOD(limits, RangeValues, RangeValues(0,0))
    EXPAND_METHOD2(limitsWrap, RangeValues, RangeValues(0,0), int, int)
    EXPAND_METHOD1(limitsAxis, QList<RangeValues>, QList<RangeValues>(), int)

    static NDArray resize1d(const NDArray &array, int size)
    {
        if (array.shape().length() > 1) {
            qWarning() << "WARNING: ArrayUtil::resize1d:Resizing - NDArray with more than 1 dimension. This only resizes underlying memory, no reshaping will occur.";
        }
        NDArray newarray(array.type(), {size});
        if (array.size() > 0) {
            auto start = array.data();
            auto end = start + array.size() * NDArray::typesize(array.type());
            std::copy(start, end, newarray.data());
        }

        return newarray;
    }

signals:

public slots:

};

#endif // ARRAYUTIL_H

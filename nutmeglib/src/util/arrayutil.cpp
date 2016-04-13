#include "arrayutil.h"
#include "util.h"

ArrayUtil::ArrayUtil(QObject *parent)
    : QObject(parent)
{

}

//template <typename T>
//qreal ArrayUtil::min(const NDArrayTyped<T> &array)
//{
//    auto it = array.cbegin();
//    auto end = array.cend();
//    T val = *it;
//    while (++it != end)
//        val = qMin(val, *it);

//    return val;
//}

//template <typename T>
//qreal ArrayUtil::max(const NDArrayTyped<T> &array)
//{
//    auto it = array.cbegin();
//    auto end = array.cend();
//    T val = *it;
//    while (++it != end)
//        val = qMax(val, *it);

//    return val;
//}

//template <typename T>
//Range ArrayUtil::limits(const NDArrayTyped<T> &array)
//{
//    auto it = array.cbegin();
//    auto end = array.cend();
//    T min_val = *it;
//    T max_val = *it;
//    while (++it != end) {
//        min_val = qMin(min_val, *it);
//        max_val = qMax(max_val, *it);
//    }

//    return Range(min_val, max_val);
//}

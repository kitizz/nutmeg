#include "util/ndarray.h"
#include "util/arrayutil.h"
#include "qttypeprinters.h"
#include "util.h"

#include <gtest/gtest.h>
#include <QTest>

#include <QList>
#include <QDebug>

using namespace ::testing;

TEST(NDArrayTest, CreateDestroy) {
    NDArray *invalid = new NDArray();
    EXPECT_NO_THROW( delete invalid );

    QList<qreal> values;
    values << 10 << 20 << 30;
    NDArray *dataref = new NDArray(values);
    auto shared = dataref->sharedData();
    qreal *dptr = (qreal*)dataref->data();

    EXPECT_EQ(shared.data()->ref, 2);

    NDArray *otherref = new NDArray(*dataref);
    EXPECT_EQ(shared.data()->ref, 3);

    delete dataref;
    EXPECT_EQ(shared.data()->ref, 2);

    delete otherref;
    EXPECT_EQ(shared.data()->ref, 1);

    EXPECT_EQ(dptr[0], values[0]);
    EXPECT_EQ(dptr[1], values[1]);
    EXPECT_EQ(dptr[2], values[2]);
}

TEST(NDArrayTest, Empty) {
    QList<qreal> values;
    QList<qreal> shape1d;
    shape1d << 0;

    NDArray arr1(values);
    ArrayUtil::min(arr1);
}

TEST(NDArrayTest, OneDim) {
    QList<int> shape;
    shape << 4;
    NDArrayTyped<int> array(shape);

    QList<int> values;
    values << 0 << 10 << -1 << 5;

    for (int i=0; i < shape[0]; ++i) {
        array(i) = values[i];
        int ind = array.get_ind({i,});
        EXPECT_EQ(ind, i);
        EXPECT_EQ(array(i), values[i]);
    }
}

TEST(NDArrayTest, OneDimExceed) {
    QList<int> shape;
    shape << 4;
    NDArrayTyped<int> array(shape);

    EXPECT_THROW(array(4), AccessException);
    EXPECT_THROW(array(-5), AccessException);
}

TEST(NDArrayTest, OneDimIterate) {
    QList<int> shape;
    shape << 4;
    NDArrayTyped<int> array(shape);

    QList<int> values;
//    values << 500 << 1 << -34 << 3;

    for (int i=0; i<4; ++i) {
        int val = qrand() % 1000 - 500;
        values << val;
        array(i) = val;
    }

    int i = 0;
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        EXPECT_EQ(*iter, values[i]);
        ++i;
    }
    EXPECT_EQ(i, shape[0]);
}

TEST(NDArrayTest, TwoDim) {
    QList<int> shape;
    shape << 4 << 9;
    NDArrayTyped<int> array(shape);

    QList<int> rows, cols;
    rows << 0 << 1 << 2 << 3;
    cols << 0 << 2 << 0 << 8;

    QList<int> values;
    values << 0 << 10 << -1 << 5;

    for (int n=0; n < values.length(); ++n) {
        int i = rows[n], j = cols[n];

        int lin_ind = array.get_ind({i,j});
        EXPECT_EQ(lin_ind, shape[1]*i + j);

        array(i, j) = values[n];
        EXPECT_EQ(array(i,j), values[n]);
    }
}

TEST(NDArrayTest, TwoDimExceed) {
    QList<int> shape;
    shape << 4 << 9;
    NDArrayTyped<int> array(shape);

    EXPECT_THROW(array(-5,0), AccessException);
    EXPECT_THROW(array(-5,-10), AccessException);
    EXPECT_THROW(array(-2,-10), AccessException);
    EXPECT_THROW(array(4,5), AccessException);
    EXPECT_THROW(array(2,9), AccessException);
    EXPECT_THROW(array(4,9), AccessException);
}

TEST(NDArrayTest, TwoDimIterate) {
    NDArrayTyped<int> array({3,3});
    QList<int> shape({3,3});

    EXPECT_EQ(array.shape(), shape);

    QList<int> values;
    for (int i=0; i<3; ++i) {
        for (int j=0; j<3; ++j) {
            int val = qrand() % 1000 - 500;
            values << val;
            array(i,j) = val;
        }
    }

    qDebug() << "Values:" << values;
    int i = 0;
    for (auto iter = array.begin(); iter != array.end(); ++iter) {
        ASSERT_LT(i, 3*3);
        EXPECT_EQ(*iter, values[i]);
        qDebug() << "Iter" << i << "=" << *iter;
        ++i;
    }
    EXPECT_EQ(i, 3*3);
}

TEST(NDArrayTest, TwoDimIterateBenchmark) {
    NDArrayTyped<int> array({1000,1000});

    BENCHMARK(TwoDimIterateFast) {
        std::memset(array.data(), 0, array.size());
    }

    BENCHMARK(TwoDimIterate) {
        auto end = array.end();
        for (auto iter = array.begin(); iter != end; ++iter) {
            *iter = 0;
        }
    }
}

TEST(NDArrayTest, OneDimListInit) {
    int N = 4;
    QList<qreal> values;
    for (int i=0; i<N; ++i) {
        qreal val = qrand() % 1000 - 500;
        values << val;
    }

    NDArray array(values);
    ASSERT_EQ(array.shape().length(), 1);
    ASSERT_EQ(array.shape()[0], N);

    for (int i=0; i<N; ++i) {
        EXPECT_EQ(values[i], array.at<qreal>(i));
    }
}

TEST(NDArrayTest, OneDimLimWrap) {
    QList<qreal> values;
    values << 10 << -10 << 100 << -100 << 5 << 20;
    NDArray array(values);

    RangeValues range = ArrayUtil::limitsWrap(array, 4, 4);

    EXPECT_EQ(range.min, -10.0);
    EXPECT_EQ(range.max, 20.0);
}

TEST(NDArrayTest, TwoDimLimits) {
    QList<qreal> values;
    values << 0 << 1 << 0 <<
              10 << -10 << 4 <<
              100 << 2 << 4 <<
              3 << -3 << 0;
    NDArray array(values);
}

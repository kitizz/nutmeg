#include "tst_QKDTreeTests.h"

#include "QKDTree.h"

#include <limits>

const uint size1 = 32000;
const uint size2 = 64000;

QKDTreeTests::QKDTreeTests()
{
}

void QKDTreeTests::lineTest()
{
    QKDTree tree(2);
    for (int i=0; i<20; ++i) {
        tree.add(QPointF(i % 5, i % 3), i);
    }

    tree.debugPrint();
}

//private test
void QKDTreeTests::insertionTest()
{
    QKDTree tree(2);
    tree.add(QPointF(0,0), 1);
    tree.add(QPointF(2,5), 3);
    tree.add(QPointF(-1,5), 2);
    tree.add(QPointF(1,6), 7);
    tree.add(QPointF(1,-2), 6);
    tree.add(QPointF(-3,7), 5);
    tree.add(QPointF(-1,4), 4);

    QKDTreeNode nearest;
    tree.nearestNode(QPointF(-1,5), &nearest);
    QVERIFY(nearest.position() == QVectorND(QPoint(-1,5)));

    //tree.debugPrint();
}

//private test
void QKDTreeTests::containsKeyTest()
{
    QList<QVectorND> ref;

    QKDTree tree(3);

    for (int i = 0; i < 5000; i++)
    {
        QVectorND random = _randomNDimensional(3);

        ref.append(random);
        QVERIFY(tree.add(random,i));
    }

    for (int i = 0; i < 5000; i++)
        QVERIFY(tree.containsKey(ref[qrand() % ref.size()]));

    for (int i = 0; i < 5000; i++)
    {
        QVectorND random = _randomNDimensional(3);
        QVERIFY(tree.containsKey(random) == ref.contains(random));
    }
}

//private test
void QKDTreeTests::valueTest()
{
    QKDTree tree(2);

    tree.add(QPointF(0,0), 1);
    tree.add(QPointF(-1,1), 2);
    tree.add(QPointF(1,-1), 3);
    tree.add(QPointF(-2,2), 4);
    tree.add(QPointF(2,-2), 5);
    tree.add(QPointF(-3,3), 6);
    tree.add(QPointF(3,-3), 7);

    QVariant val;
    QVERIFY(tree.value(QPointF(0,0), &val));
    QVERIFY(val == 1);

    QVERIFY(tree.value(QPointF(-1,1), &val));
    QVERIFY(val == 2);

    QVERIFY(tree.value(QPointF(1,-1), &val));
    QVERIFY(val == 3);

    QVERIFY(tree.value(QPointF(-2,2), &val));
    QVERIFY(val == 4);

    QVERIFY(tree.value(QPointF(2,-2), &val));
    QVERIFY(val == 5);

    QVERIFY(tree.value(QPointF(-3,3), &val));
    QVERIFY(val == 6);

    QVERIFY(tree.value(QPointF(3,-3), &val));
    QVERIFY(val == 7);

    QVERIFY(!tree.value(QPointF(50,50), &val));
}

//private test
void QKDTreeTests::bigNearestTest()
{
    const int dim = 4;
    const int count = 5000;
    QList<QVectorND> backupList;
    QKDTree tree(dim);

    for (int i = 0; i < count; i++)
    {
        QVectorND pos = _randomNDimensional(dim);

        backupList.append(pos);
        QVERIFY(tree.add(pos, i));
    }

    QVERIFY(backupList.size() == tree.size());

    bool wasError = false;
    for (int i = 0; i < count; i++)
    {
        QVectorND pos = _randomNDimensional(dim);

        QKDTreeNode nearest;
        tree.nearestNode(pos, &nearest);
        qreal treeDistance = tree.distanceMetric()->distance(pos,nearest.position());

        QVectorND backupNearest(dim);
        qreal backupDist = std::numeric_limits<qreal>::max();
        foreach(const QVectorND& vec, backupList)
        {
            qreal dist = tree.distanceMetric()->distance(vec, pos);
            if (dist < backupDist)
            {
                backupNearest = vec;
                backupDist = dist;
            }
        }
        qreal listDistance = tree.distanceMetric()->distance(pos,backupNearest);

        if (nearest.position() != backupNearest && treeDistance != listDistance)
        {
            qDebug() << "Tree: Nearest to" << pos << "is" << nearest.position() << treeDistance;
            qDebug() << "List: Nearest to" << pos << "is" << backupNearest << listDistance;
            wasError = true;
        }
    }

    if (wasError)
        QVERIFY(false);
}

void QKDTreeTests::nearestPosByPosTest()
{
    const int dim = 3;
    const int count = 5000;
    QList<QVectorND> refList;
    QKDTree tree(dim);

    for (int i = 0; i < count; i++)
    {
        const QVectorND pos = _randomNDimensional(dim);
        refList.append(pos);
        QVERIFY(tree.add(pos,i));
    }

    bool wasError = false;
    for (int i = 0; i < count; i++)
    {
        const QVectorND searchPoint = _randomNDimensional(dim);

        QVectorND nearestByTree(dim);
        QVERIFY(tree.nearestKey(searchPoint, &nearestByTree));
        const qreal treeDist = tree.distanceMetric()->distance(nearestByTree, searchPoint);

        QVectorND nearestByList(dim);
        qreal bestDist = std::numeric_limits<qreal>::max();
        foreach(const QVectorND& candidate, refList)
        {
            const qreal dist = tree.distanceMetric()->distance(candidate, searchPoint);
            if (dist < bestDist)
            {
                bestDist = dist;
                nearestByList = candidate;
            }
        }

        if (bestDist != treeDist)
        {
            qDebug() << "Nearest to" << searchPoint << "by tree:" << nearestByTree << treeDist;
            qDebug() << "Nearest to" << searchPoint << "by list:" << nearestByList << bestDist;
        }
    }

    if (wasError)
        QVERIFY(false);
}

//private test
void QKDTreeTests::benchmarkTreeAdd1()
{
    QKDTree tree(2);
    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, "final");
    }
}

//private test
void QKDTreeTests::benchmarkTreeAdd2()
{
    QKDTree tree(2);
    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, "final");
    }
}

//private test
void QKDTreeTests::benchmarkTreeNearest1()
{
    QKDTree tree(2);
    QList<QVectorND> results;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QKDTreeNode nearestResult;
        QBENCHMARK
        {
            tree.nearestNode(pos, &nearestResult);
        }

        results.append(nearestResult.position());
    }
}

//private test
void QKDTreeTests::benchmarkTreeNearest2()
{
    QKDTree tree(2);
    QList<QVectorND> results;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        tree.add(pos, i);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QKDTreeNode nearestResult;
        QBENCHMARK
        {
            tree.nearestNode(pos, &nearestResult);
        }

        results.append(nearestResult.position());
    }
}

//private test
void QKDTreeTests::benchmarkListAdd1()
{
    QList<QVectorND> list;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }
}

//private test
void QKDTreeTests::benchmarkListAdd2()
{
    QList<QVectorND> list;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    QBENCHMARK
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }
}

//private test
void QKDTreeTests::benchmarkListNearest1()
{
    QList<QVectorND> list;
    QList<QVectorND> results;

    for (int i = 0; i < size1; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QVectorND best(1);
        QBENCHMARK
        {
            qreal bestDist = std::numeric_limits<qreal>::max();
            foreach(const QVectorND& vec, list)
            {
                const qreal dist = (vec - pos).lengthSquared();
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = vec;
                }
            }
        }
        results.append(best);
    }
}

//private test
void QKDTreeTests::benchmarkListNearest2()
{
    QList<QVectorND> list;
    QList<QVectorND> results;

    for (int i = 0; i < size2; i++)
    {
        QVectorND pos = _randomNDimensional(2);
        list.append(pos);
    }

    for (int i = 0; i < 1; i++)
    {
        QVectorND pos = _randomNDimensional(2);

        QVectorND best(1);
        QBENCHMARK
        {
            qreal bestDist = std::numeric_limits<qreal>::max();
            foreach(const QVectorND& vec, list)
            {
                const qreal dist = (vec - pos).lengthSquared();
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = vec;
                }
            }
        }
        results.append(best);
    }
}

//private static
QVectorND QKDTreeTests::_randomNDimensional(int n)
{
    QVectorND toRet(n);

    for (int i = 0; i < n; i++)
        toRet[i] = qrand();

    return toRet;
}

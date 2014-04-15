#ifndef QKDTREE_H
#define QKDTREE_H

#include "QKDTree_global.h"

#include "QKDTreeNode.h"
#include "QKDTreeDistanceMetric.h"
#include "../QVectorND/QVectorND.h"

class QKDTREESHARED_EXPORT QKDTree
{
public:
    /**
     * @brief QKDTree constructs a kd-tree that takes positions of the given dimension.
     * e.g., to store 2d points, set dimension = 2.
     * @param dimension
     * @param allowDuplocates whether or not you can add multiple nodes with the same key
     * @param distanceMetric the custom distance metric object you would like to use. If 0 euclidean
     * distance squared is used.
     */
    QKDTree(int dimension, bool allowDuplicates = false, QKDTreeDistanceMetric * distanceMetric = 0);
    ~QKDTree();

    /**
     * @brief dimension Returns the dimensionality of the positions stored by this tree.
     * @return
     */
    int dimension() const;

    /**
     * @brief size Returns the number of key/value pairs in the tree
     * @return
     */
    qint64 size() const;

    bool add(QKDTreeNode * node, QString * resultOut = 0);
    bool add(const QVectorND& position, const QVariant& value, QString * resultOut = 0);
    bool add(const QPointF& position, const QVariant& value, QString * resultOut = 0);

    bool nearestNode(const QVectorND& position, QKDTreeNode * output, QString * resultOut = 0);
    bool nearestNode(const QPointF& position, QKDTreeNode * output, QString * resultOut = 0);
    bool nearestNode(QKDTreeNode * node, QKDTreeNode * output, QString * resultOut = 0);

    bool nearestKey(const QVectorND& position, QVectorND * output, QString * resultOut = 0);

    bool containsKey(const QVectorND& position);
    bool containsKey(QKDTreeNode * node);

    /**
     * @brief value returns the value of the first node found with the given key
     * @param positionKey
     * @param output
     * @param resultOut
     * @return
     */
    bool value(const QVectorND& positionKey, QVariant * output, QString * resultOut = 0);
    bool value(const QPointF& positionKey, QVariant * output, QString * resultOut = 0);

    QKDTreeDistanceMetric * distanceMetric() const;

    /**
     * @brief debugPrint does a breadth-first search of the tree, printing values as it goes.
     */
    void debugPrint();

private:
    int _dimension;
    qint64 _size;

    QKDTreeNode * _root;

    bool _allowDuplicates;
    QKDTreeDistanceMetric * _distanceMetric;
};

#endif // QKDTREE_H

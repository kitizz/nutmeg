#ifndef QKDTREEDISTANCEMETRIC_H
#define QKDTREEDISTANCEMETRIC_H

#include "QKDTreeNode.h"

#include "QKDTree_global.h"

/**
 * @brief The QKDTreeDistanceMetric class is used by QKDTree to calculate distances between positions.
 * This base implementation returns squared euclidean distance.
 */
class QKDTREESHARED_EXPORT QKDTreeDistanceMetric
{
public:
    QKDTreeDistanceMetric();
    virtual ~QKDTreeDistanceMetric();

    qreal distance(const QKDTreeNode * const a, const QKDTreeNode * const b);

    /**
     * @brief distance This method returns the distance between two positions in the tree.
     * To use a custom distance metric, create an inheriting class that overrides this method and
     * pass the child class to your tree using QKDTree::setDistanceMetric
     * @param a
     * @param b
     * @return
     */
    virtual qreal distance(const QVectorND& a, const QVectorND& b);
};

#endif // QKDTREEDISTANCEMETRIC_H

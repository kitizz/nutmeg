#include "QKDTreeDistanceMetric.h"

#include <QtGlobal>
#include <QtDebug>
#include <cmath>

QKDTreeDistanceMetric::QKDTreeDistanceMetric()
{
}

QKDTreeDistanceMetric::~QKDTreeDistanceMetric()
{
}

qreal QKDTreeDistanceMetric::distance(const QKDTreeNode *const a, const QKDTreeNode *const b)
{
    return this->distance(a->position(), b->position());
}

//virtual - this one returns euclidean distance
qreal QKDTreeDistanceMetric::distance(const QVectorND &a, const QVectorND &b)
{
    return (a - b).lengthSquared();
}

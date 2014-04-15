#include "QKDTreeNode.h"

#include <QtDebug>

QKDTreeNode::QKDTreeNode(const QVectorND &position, const QVariant &value) :
    _position(position), _value(value), _left(0), _right(0), _dividingDimension(0)
{
}

QKDTreeNode::~QKDTreeNode()
{
}

const QVectorND &QKDTreeNode::position() const
{
    return _position;
}

const QVariant &QKDTreeNode::value() const
{
    return _value;
}

void QKDTreeNode::setValue(const QVariant &nVal)
{
    _value = nVal;
}

//private
QKDTreeNode *QKDTreeNode::left() const
{
    return _left;
}

//private
QKDTreeNode *QKDTreeNode::right() const
{
    return _right;
}

//private
void QKDTreeNode::setLeft(QKDTreeNode *nLeft)
{
    _left = nLeft;
}

//private
void QKDTreeNode::setRight(QKDTreeNode *nRight)
{
    _right = nRight;
}

//private
int QKDTreeNode::dividingDimension() const
{
    return _dividingDimension;
}

//private
void QKDTreeNode::setDividingDimension(int nDiv)
{
    _dividingDimension = nDiv;
}

#ifndef QKDTREENODE_H
#define QKDTREENODE_H

#include "../QVectorND/QVectorND.h"

#include "QKDTree_global.h"

//forward definition so that we can make QKDTree a "friend" class of QKDTreeNode
class QKDTree;

class QKDTREESHARED_EXPORT QKDTreeNode
{
public:
    QKDTreeNode(const QVectorND& position = QVectorND(1), const QVariant& value = QVariant());
    ~QKDTreeNode();

    const QVectorND& position() const;

    const QVariant& value() const;
    void setValue(const QVariant& nVal);

private:
    QKDTreeNode *left() const;
    QKDTreeNode *right() const;
    void setLeft(QKDTreeNode * nLeft);
    void setRight(QKDTreeNode * nRight);

    int dividingDimension() const;
    void setDividingDimension(int nDiv);

private:
    QVectorND _position;
    QVariant _value;

    QKDTreeNode * _left;
    QKDTreeNode * _right;
    int _dividingDimension;

    friend class QKDTree;
};

#endif // QKDTREENODE_H

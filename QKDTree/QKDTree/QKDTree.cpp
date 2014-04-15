#include "QKDTree.h"

#include <QQueue>
#include <QStack>
#include <QtDebug>
#include <limits>


const QString ERR_STRING_BAD_DIM = "Dimension of position does not match that of tree.";
const QString ERR_STRING_BAD_OUTPTR = "You didn't provide a pointer for output.";

QKDTree::QKDTree(int dimension, bool allowDuplicates, QKDTreeDistanceMetric *distanceMetric) :
    _dimension(dimension), _size(0), _root(0), _allowDuplicates(allowDuplicates)
{
    //If they don't give us a distance metric, just use the default
    _distanceMetric = distanceMetric;
    if (_distanceMetric == 0)
        _distanceMetric = new QKDTreeDistanceMetric();
}

QKDTree::~QKDTree()
{
    if (_size > 0)
    {
        QQueue<QKDTreeNode *> deleteQueue;
        deleteQueue.enqueue(_root);

        while (!deleteQueue.isEmpty())
        {
            QKDTreeNode * current = deleteQueue.dequeue();
            if (current->left())
                deleteQueue.enqueue(current->left());
            if (current->right())
                deleteQueue.enqueue(current->right());
            delete current;
        }

        _root = 0;
        _size = 0;
    }
    delete _distanceMetric;
}

int QKDTree::dimension() const
{
    return _dimension;
}

qint64 QKDTree::size() const
{
    return _size;
}

bool QKDTree::add(QKDTreeNode *node, QString *resultOut)
{
    if (node == 0)
    {
        if (resultOut)
            *resultOut = "Cannot add null node";
        return false;
    }
    else if (node->position().dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }

    //Special case for first node in the tree!
    if (_root == 0)
    {
        _root = node;
        _size++;
        return true;
    }

    //Otherwise, normal insertion
    QKDTreeNode * potentialParent = _root;
    while (true)
    {
        const int divDim = potentialParent->dividingDimension();
        if (!_allowDuplicates && node->position() == potentialParent->position())
        {
            if (resultOut)
                *resultOut = "Cannot add duplicate";
            return false;
        }
        else if (node->position().val(divDim) <= potentialParent->position().val(divDim))
        {
            if (potentialParent->left() != 0)
            {
                potentialParent = potentialParent->left();
            }
            else
            {
                potentialParent->setLeft(node);
                node->setDividingDimension((divDim + 1) % this->dimension());
                break;
            }
        }
        else
        {
            if (potentialParent->right() != 0)
            {
                potentialParent = potentialParent->right();
            }
            else
            {
                potentialParent->setRight(node);
                node->setDividingDimension((divDim + 1) % this->dimension());
                break;
            }
        }
    }

    _size++;
    return true;
}

bool QKDTree::add(const QVectorND &position, const QVariant &value, QString *resultOut)
{
    QKDTreeNode * node = new QKDTreeNode(position, value);

    return this->add(node, resultOut);
}

bool QKDTree::add(const QPointF &position, const QVariant &value, QString *resultOut)
{
    QKDTreeNode * node = new QKDTreeNode(QVectorND(position), value);

    return this->add(node, resultOut);
}


bool QKDTree::nearestNode(const QVectorND &searchPos, QKDTreeNode *output, QString *resultOut)
{
    if (output == 0)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_OUTPTR;
        return false;
    }
    else if (searchPos.dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }
    else if (_size <= 0)
    {
        if (resultOut)
            *resultOut = "Tree is empty";
        return false;
    }

    foreach (qreal v, searchPos.values()) {
        if (v != v) {
            if (resultOut)
                *resultOut = "NaN values in search position";
            return false;
        }
    }

    QQueue<QKDTreeNode *> descend;
    QStack<QKDTreeNode *> unwindChecks;

    descend.enqueue(_root);

    QKDTreeNode * bestSoFar = 0;
    qreal bestDistSoFar = std::numeric_limits<qreal>::max();

    while (!descend.isEmpty() || !unwindChecks.isEmpty())
    {
        if (!descend.isEmpty())
        {
            QKDTreeNode * current = descend.dequeue();
            unwindChecks.push(current);

            const int divDim = current->dividingDimension();
            if (searchPos.val(divDim) <= current->position().val(divDim))
            {
                if (current->left() != 0)
                    descend.enqueue(current->left());
                else
                {
                    const qreal dist = _distanceMetric->distance(current->position(), searchPos);

                    if (dist < bestDistSoFar)
                    {
                        bestSoFar = current;
                        bestDistSoFar = dist;
                    }
                }
            }
            else
            {
                if (current->right() != 0)
                    descend.enqueue(current->right());
                else
                {
                    const qreal dist = _distanceMetric->distance(current->position(), searchPos);

                    if (dist < bestDistSoFar)
                    {
                        bestSoFar = current;
                        bestDistSoFar = dist;
                    }
                }
            }
        }
        else
        {
            //In this branch we "unwind" up the tree, checking those nodes for nearer-ness
            QKDTreeNode * current = unwindChecks.pop();
            const int divDim = current->dividingDimension();
            const qreal dist = _distanceMetric->distance(current->position(), searchPos);

            if (dist < bestDistSoFar)
            {
                bestSoFar = current;
                bestDistSoFar = dist;
            }

            //Do we need to check other side of hyperplane?
            QVectorND temp = current->position();
            temp[divDim] = searchPos.val(divDim);
            const qreal hyperplaneDistance = this->distanceMetric()->distance(temp, current->position());
            if (hyperplaneDistance > bestDistSoFar)
                continue;

            //Search the other side of the dividing node
            if (searchPos.val(divDim) <= current->position().val(divDim)
                    && current->right() != 0)
                descend.enqueue(current->right());
            else if (current->left() != 0)
                descend.enqueue(current->left());

        }
    }

    *output = *bestSoFar;

    return true;
}

bool QKDTree::nearestNode(const QPointF &position, QKDTreeNode *output, QString *resultOut)
{
    return this->nearestNode(QVectorND(position), output, resultOut);
}

bool QKDTree::nearestNode(QKDTreeNode *node, QKDTreeNode *output, QString *resultOut)
{
    return this->nearestNode(node->position(), output, resultOut);
}

bool QKDTree::nearestKey(const QVectorND &position, QVectorND *output, QString *resultOut)
{
    if (output == 0)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_OUTPTR;
        return false;
    }

    QKDTreeNode nearestNode;
    if (!this->nearestNode(position, &nearestNode, resultOut))
        return false;

    *output = nearestNode.position();

    return true;
}

bool QKDTree::containsKey(const QVectorND &position)
{
    if (position.dimension() != this->dimension())
        return false;
    else if (_size <= 0)
        return false;

    QKDTreeNode * current = _root;

    while (current != 0)
    {
        if (current->position() == position)
            return true;

        const int divDim = current->dividingDimension();
        if (position.val(divDim) <= current->position().val(divDim))
            current = current->left();
        else
            current = current->right();
    }
    return false;
}

bool QKDTree::containsKey(QKDTreeNode *node)
{
    if (node == 0)
        return false;

    return this->containsKey(node->position());
}

bool QKDTree::value(const QVectorND &positionKey, QVariant *output, QString *resultOut)
{
    const QString errStringNotFound = "Key not found";

    if (output == 0)
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_OUTPTR;
        return false;
    }
    else if (this->size() <= 0)
    {
        if (resultOut)
            *resultOut = errStringNotFound;
        return false;
    }
    else if (positionKey.dimension() != this->dimension())
    {
        if (resultOut)
            *resultOut = ERR_STRING_BAD_DIM;
        return false;
    }

    QKDTreeNode * current = _root;
    while (current != 0)
    {
        const int divDim = current->dividingDimension();

        if (current->position() == positionKey)
        {
            *output = current->value();
            return true;
        }
        else if (positionKey.val(divDim) <= current->position().val(divDim))
            current = current->left();
        else
            current = current->right();
    }

    if (resultOut)
        *resultOut = errStringNotFound;
    return false;
}

bool QKDTree::value(const QPointF &positionKey, QVariant *output, QString *resultOut)
{
    return this->value(QVectorND(positionKey), output, resultOut);
}

QKDTreeDistanceMetric *QKDTree::distanceMetric() const
{
    return _distanceMetric;
}

void QKDTree::debugPrint()
{
    if (_size <= 0)
        return;

    QList<QKDTreeNode *> *firstStack = new QList<QKDTreeNode*>();
    firstStack->append(_root);

    QList<QList< QKDTreeNode *>* > childListStack;
    childListStack.append(firstStack);

    while (childListStack.length() > 0)
    {
        QList<QKDTreeNode *> *childStack = childListStack.last();

        if (childStack->length() == 0) {
            childListStack.removeLast();
            delete childStack;
        } else {
            QKDTreeNode * n = childStack->takeFirst();

            QString indent;
            for (int i=0; i < childListStack.length() - 1; ++i) {
                indent += childListStack[i]->length() > 0 ? "| " : "  ";
            }
            indent += "+-";

            QList<QKDTreeNode *> *newStack = new QList<QKDTreeNode *>();
            if (n->left())
                newStack->append(n->left());
            if (n->right())
                newStack->append(n->right());

            childListStack.append(newStack);

            qDebug() << qPrintable(indent) << n->position() << n->value();
        }
    }
}

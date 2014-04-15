#include "QVectorND.h"

#include <QtDebug>
#include <cmath>

QVectorND::QVectorND(int dimensions) :_dimensions(dimensions)
{
    for (int i = 0; i < _dimensions; i++)
        _data.append(0.0);
}

QVectorND::QVectorND(const QList<qreal> &values)
{
    _dimensions = values.size();
    for (int i = 0; i < values.size(); i++)
        _data.append(values[i]);
}

QVectorND::QVectorND(const QPoint &point)
{
    _dimensions = 2;
    _data.append(point.x());
    _data.append(point.y());
}

QVectorND::QVectorND(const QPointF &point)
{
    _dimensions = 2;
    _data.append(point.x());
    _data.append(point.y());
}

QVectorND::QVectorND(const QVector2D &vec)
{
    _dimensions = 2;
    _data.append(vec.x());
    _data.append(vec.y());
}

QVectorND::QVectorND(const QVector3D &vec)
{
    _dimensions = 3;
    _data.append(vec.x());
    _data.append(vec.y());
    _data.append(vec.z());
}

QVectorND::QVectorND(const QVector4D &vec)
{
    _dimensions = 4;
    _data.append(vec.x());
    _data.append(vec.y());
    _data.append(vec.z());
    _data.append(vec.w());
}

QVectorND::QVectorND(const QVectorND &other)
{
    _dimensions = other._dimensions;
    _data = other._data;
}

int QVectorND::dimension() const
{
    return _dimensions;
}

bool QVectorND::isNull() const
{
    bool toRet = true;
    foreach(qreal val, _data)
    {
        if (val != 0.0)
        {
            toRet = false;
            break;
        }
    }
    return toRet;
}

qreal QVectorND::length() const
{
    return sqrt(this->lengthSquared());
}

qreal QVectorND::lengthSquared() const
{
    qreal toRet = 0.0;
    for (int i = 0; i < _dimensions; i++)
        toRet += _data[i] * _data[i];
    return toRet;
}

qreal QVectorND::manhattanDistance() const
{
    qreal toRet = 0.0;
    for (int i = 0; i < _dimensions; i++)
        toRet += qAbs<qreal>(_data[i]);
    return toRet;
}

void QVectorND::normalize()
{
    const qreal length = this->length();
    if (length < 0.00000001)
        return;

    for(int i = 0; i < _dimensions; i++)
        _data[i] /= length;
}

QVectorND QVectorND::normalized() const
{
    QVectorND copy(*this);
    copy.normalize();
    return copy;
}

void QVectorND::setVal(int index, qreal value)
{
    if (index < 0 || index >= _dimensions)
    {
        qWarning() << "QVectorND setVal() index out of bounds" << index;
        return;
    }

    _data[index] = value;
}

qreal QVectorND::val(int index) const
{
    if (index < 0 || index >= _dimensions)
    {
        qWarning() << "QVectorND val() index out of bounds" << index;
        return 0.0;
    }

    return _data.at(index);
}

const QVector<qreal> &QVectorND::values() const
{
    return _data;
}

QVectorND &QVectorND::operator *=(qreal factor)
{
    for (int i = 0; i < _dimensions; i++)
        _data[i] *= factor;
    return *this;
}

QVectorND &QVectorND::operator *=(const QVectorND &other)
{
    if (other.dimension() != this->dimension())
    {
        qWarning() << "Can't multiply our" << this->dimension() << "dimensional vector by a" << other.dimension() << "dimensional one.";
        return *this;
    }

    for (int i = 0; i < _dimensions; i++)
        _data[i] *= other._data[i];

    return *this;
}

QVectorND &QVectorND::operator +=(const QVectorND &other)
{
    if (other.dimension() != this->dimension())
    {
        qWarning() << "Can't add our" << this->dimension() << "dimensional vector with a" << other.dimension() << "dimensional one.";
        return *this;
    }

    for (int i = 0; i < _dimensions; i++)
        _data[i] += other._data[i];

    return *this;
}

QVectorND &QVectorND::operator -=(const QVectorND &other)
{
    if (other.dimension() != this->dimension())
    {
        qWarning() << "Can't subtract our" << this->dimension() << "dimensional vector by a" << other.dimension() << "dimensional one.";
        return *this;
    }

    for (int i = 0; i < _dimensions; i++)
        _data[i] -= other._data[i];

    return *this;
}

QVectorND &QVectorND::operator /=(qreal divisor)
{
    for (int i = 0; i < _dimensions; i++)
        _data[i] /= divisor;
    return *this;
}

bool QVectorND::operator ==(const QVectorND &other) const
{
    if (other.dimension() != this->dimension())
        return false;

    for (int i = 0; i < _dimensions; i++)
    {
        if (_data[i] != other._data[i])
            return false;
    }
    return true;
}

bool QVectorND::operator !=(const QVectorND &other) const
{
    return !(other == *this);
}

qreal &QVectorND::operator [](int index)
{
    if (index < 0 || index >= _dimensions)
        qCritical("Index out of bounds");

    return _data[index];
}

qreal QVectorND::operator [](int index) const
{
    if (index < 0 || index >= _dimensions)
        qCritical("Index out of bounds");

    return _data[index];
}


//non-member
uint qHash(const QVectorND& vec)
{
    uint toRet = 0;

    for (int i = 0; i < vec.dimension(); i++)
        toRet = toRet ^ (uint)vec[i];

    return toRet;
}

//non-member
QDebug operator<<(QDebug dbg, const QVectorND& vec)
{
    dbg.nospace() << "(";
    for(int i = 0; i < vec.dimension(); i++)
    {
        dbg.nospace() << vec.val(i);
        if (i < vec.dimension() - 1)
            dbg.nospace() << ",";
    }

    dbg.nospace() << ")";

    return dbg.space();
}

//non-member
const QVectorND operator-(const QVectorND& v1, const QVectorND& v2)
{
    QVectorND toRet = v1;
    toRet -= v2;
    return toRet;
}

//non-member
const QVectorND operator-(const QVectorND& v)
{
    QVectorND toRet = v;
    for (int i = 0; i < toRet.dimension(); i++)
        toRet[i] *= -1;
    return toRet;
}

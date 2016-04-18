#include "ndarray.h"
#include "util.h"

#include <QDebug>

// These template definitions allow a type to be converted to its associated enum
template <typename T> const NDArray::Type NDArray::TypeMap<T>::type = NDArray::Unknown;
template <> const NDArray::Type NDArray::TypeMap<int>::type = NDArray::Int64;
template <> const NDArray::Type NDArray::TypeMap<uint8_t>::type = NDArray::Uint8;
template <> const NDArray::Type NDArray::TypeMap<double>::type = NDArray::Float64;
template <> const NDArray::Type NDArray::TypeMap<float>::type = NDArray::Float32;
template <> const NDArray::Type NDArray::TypeMap<bool>::type = NDArray::Bool;

ArrayData::ArrayData(int length)
    : data(new char[length]), length(length)
{
}

ArrayData::ArrayData(char* data, int length)
    : data(data), length(length)
{
}

ArrayData::ArrayData(const ArrayData &other)
    : data(other.data), length(other.length)
{
}

ArrayData::~ArrayData()
{
    if (data)
        delete data;
}

NDArray::NDArray()
    : NDArray(Unknown, QList<int>())
{
}

NDArray::NDArray(const QList<qreal> &values)
    : NDArray(TypeMap<qreal>::type, {values.length()}, 0)
{
    qreal* dptr = (qreal*)data();
    // Copy in data from list
    for (int i=0; i<m_size; ++i)
        (*dptr++) = values[i];
}

NDArray::NDArray(const QVariantList &values)
    : NDArray(TypeMap<qreal>::type, {values.length()}, 0)
{
    qreal* dptr = (qreal*)data();
    // Copy in data from list
    for (int i=0; i<m_size; ++i) {
        qDebug() << QString("%1: %2").arg(i).arg(values[i].toReal());
        (*dptr++) = values[i].toReal();
    }
}

NDArray::NDArray(Type type, std::initializer_list<int> shape, char *dptr)
    : NDArray(type, QList<int>(shape), dptr)
{
}

NDArray::NDArray(NDArray::Type type, const QList<int> &shape, const QByteArray &bytes)
    : NDArray(type, shape, 0)
{
    qDebug() << "New Array. Shape:" << shape;
    if (m_type == Unknown)
        return;
    // Make a copy of the ByteArray. It seems for now that nzmqt takes ownership of it
    // TODO: See if we can modify nzmqt to not delete bytearray data
    char *data = (char*)bytes.data();
    int size = m_size * m_typesize;
    std::copy(data, data + size, m_data_ch);
}

NDArray::NDArray(Type type, const QList<int> &shape, char *dptr)
    : m_type(type)
    , m_shapelst(shape)
    , m_shape(QSharedPointer<int>())
    , m_strides(QSharedPointer<int>())
    , m_size(0)
    , m_typesize(typesize(type))
    , m_data_ch(0)
{
    m_ndim = shape.count();

    if (m_type == Unknown)
        return;

    // Get the total size of the allocated memory required
    int size = 1;
    foreach(int dim, shape)
        size *= dim;

    int dsize = size * m_typesize;
    if (dptr == 0) {
        dptr = new char[dsize];
    }
    m_data_ch = dptr;
    m_data_ptr = new ArrayData(dptr, dsize);

    // Get shape and offsets right
    m_shape = QSharedPointer<int>::create(m_ndim);
    m_strides = QSharedPointer<int>::create(m_ndim);
    int str = 1;
    for (int i=0; i<m_ndim; ++i) {
        m_shape.data()[i] = shape[i];
        int i_r = m_ndim - i - 1;
        m_strides.data()[i_r] = str;
        str *= shape[i_r];
    }

    update_size();
}

NDArray::NDArray(const NDArray &other)
    : m_ndim(other.m_ndim)
    , m_type(other.m_type)
    , m_shapelst(other.m_shapelst)
    , m_size(other.m_size)
    , m_typesize(other.m_typesize)
    , m_data_ptr(other.m_data_ptr)
    , m_data_ch(other.m_data_ch)
{
    m_shape = QSharedPointer<int>::create(m_ndim);
    m_strides = QSharedPointer<int>::create(m_ndim);
    std::memcpy(m_shape.data(), other.m_shape.data(), m_ndim);
    std::memcpy(m_strides.data(), other.m_strides.data(), m_ndim);
}

NDArray::~NDArray()
{
}

char *NDArray::data() const
{
    return m_data_ch;
}

int NDArray::ndim() const
{
    return m_ndim;
}

NDArray::Type NDArray::type() const
{
    return m_type;
}

const QList<int> NDArray::shape() const
{
    return m_shapelst;
}

int NDArray::size() const
{
    return m_size;
}

int NDArray::get_ind(const std::initializer_list<int> &inds)
{
    return get_ind_temp(inds);
}

int NDArray::get_ind(const QVector<int> &inds)
{
    return get_ind_temp(inds);
}

bool NDArray::check_inds(QVector<int> &inds)
{
    if (m_type == Unknown)
        return false;

    if ((int)inds.size() != m_ndim) {
        qErrnoWarning("NDArray::get() - Incorrect number of dimensions!");
        return false;
    }

    int n = m_ndim;
    auto inds_it = inds.begin();
    int *shp = m_shape.data();

    // Adjust indices as required
    while (n--) {
        const int index = *inds_it;
        const int s = *shp++;
        if (index >= s || index < -s)
            return false;

        (*inds_it++) = (index < 0) ? s + index : index;
    }

    return true;
}

void NDArray::update_size()
{
    m_size = 1;
    int n = m_ndim;
    int *shp = m_shape.data();
    while (n--)
        m_size *= (*shp++);

    if (m_size * m_typesize > m_data_ptr.data()->length) {
        qErrnoWarning("NDarray shape is too big for underlying data!");
        m_size = m_data_ptr.data()->length / m_typesize;
    }
}

template <typename Container>
int NDArray::get_ind_temp(Container &inds)
{
    int n = m_ndim;
    auto ind = inds.begin();
    int *strides = m_strides.data();
    int ind_data = 0;

    while (n--)
        ind_data += (*strides++) * (*ind++);

    return ind_data;
}

template <class T>
NDArrayTyped<T>::NDArrayTyped(const NDArrayTyped<T> &other)
    : NDArray(other)
    , m_data(other.m_data)
{
}

template <class T>
NDArrayTyped<T>::NDArrayTyped(const NDArray &other, T* dptr)
    : NDArray(other)
//    , m_data((T*)other.m_data_ch)
{
    if (!dptr)
        m_data = (T*)other.data();
    else
        m_data = dptr;
}

template <class T>
NDArrayTyped<T>::NDArrayTyped(std::initializer_list<int> shape)
    : NDArrayTyped<T>(QList<int>(shape))
{
}

template <class T>
NDArrayTyped<T>::NDArrayTyped(const QList<int> &shape)
    : NDArray(TypeMap<T>::type, shape)
{
    m_data = (T*)m_data_ch;
}

template <class T>
T &NDArrayTyped<T>::at(const std::initializer_list<int> &inds)
{
    return *(m_data + get_ind(inds));
}

template <class T>
T &NDArrayTyped<T>::at(const QVector<int> &inds)
{
    return *(m_data + get_ind(inds));
}

// Predefine NDArrayTyped template types
template class NDArrayTyped<int>;
template class NDArrayTyped<double>;
template class NDArrayTyped<float>;
template class NDArrayTyped<uint8_t>;
template class NDArrayTyped<bool>;

#ifndef NDARRAY_H
#define NDARRAY_H

#include "../nutmeglibdecl.h"
#include <QVector>
#include <QExplicitlySharedDataPointer>
#include <QException>
#include <initializer_list>
#include <iterator>
#include <QDebug>
#include <QSharedPointer>

class NUTMEGLIB_EXPORT ArrayData : public QSharedData
{
public:
    ArrayData(int length);
    ArrayData(char *data, int length);
    ArrayData(const ArrayData &other);
//    ArrayData& operator=(const ArrayData &other);
    ~ArrayData();

    char *data;
    int length;
};

class AccessException : public std::exception
{
    virtual const char* what() const throw()
    {
        return "Access Exception..";
    }
};

class InvalidType : public std::exception
{
    virtual const char* what() const throw()
    {
        return "NDArray is an invalid type";
    }
};

template <class T>
class NDArrayTyped;

class NUTMEGLIB_EXPORT NDArray
{
public:
    /*!
     * \brief The Type enum
     * \enum BinarySpec::Type
     * This enum defines integer values for the possible data types sent as binary to Nutmeg
     * Ensure that this is synchronized with the sender implementation
     */
    enum Type { Unknown=-1, Int64=0, Uint8=1, Float32=2, Float64=3, Bool=4 };
    // Note: When adding types, currently need to additonally modify the following:
    // NDArray::convert
    // NDArray::typesize
    // type->enum template definitions at top of NDArray source
    // Prefined NDArrayTyped type definitions at the bottom of NDArray source
    // BinarySpec::fromString
    // ArrayUtil::EXPAND_METHOD macro

    // TODO: Yes, this is horrible, this could probably be done with macros and loops...

    template <typename T> struct TypeMap { static const Type type; };

    NDArray();
    NDArray(const QList<qreal> &values);
    NDArray(const QVariantList &values);
    NDArray(Type type, std::initializer_list<int> shape, char *dptr=0);
    NDArray(Type type, const QList<int> &shape, const QByteArray &bytes);
    NDArray(Type type, const QList<int> &shape, char *dptr=0);
    NDArray(const NDArray &other);
    ~NDArray();

    char *data() const;

    int ndim() const;
    Type type() const;
    const QList<int> shape() const;
    int size() const;

    int get_ind(const std::initializer_list<int> &inds);
    int get_ind(const QVector<int> &inds);

    template <typename T>
    T &at(const std::initializer_list<int> &inds)
    {
        const int ind = get_ind(inds);
        if (ind > m_size)
            throw AccessException();

        return *((T*)m_data_ch + ind);
    }

    template <typename T>
    T &at(int ind)
    {
        if (ind > m_size)
            throw AccessException();

        return *((T*)m_data_ch + ind);
    }

    template <typename T>
    NDArrayTyped<T> convert() const
    {
        if (m_type == Unknown) {
            qWarning() << "Cannot convert invalid array";
            throw InvalidType();
        }

        Type type = TypeMap<T>::type;
        if (type == Unknown) {
            qWarning()  << "Unable to convert to unsupported datatype";
            throw InvalidType();
        }

        if (type == m_type) {
            return NDArrayTyped<T>(*this);

        } else {
            // Need to copy and do full conversion of underlying data
            T* dst = new T[m_size];
            switch (m_type) {
            case Int64: {
                int *src = (int*)m_data_ch;
                std::copy(src, src + m_size, dst);
                break;
            }
            case Uint8: {
                uint8_t *src = (uint8_t*)m_data_ch;
                std::copy(src, src + m_size, dst);
                break;
            }
            case Float32: {
                float *src = (float*)m_data_ch;
                std::copy(src, src + m_size, dst);
                break;
            }
            case Float64: {
                double *src = (double*)m_data_ch;
                std::copy(src, src + m_size, dst);
                break;
            }
            case Bool: {
                bool *src = (bool*)m_data_ch;
                std::copy(src, src + m_size, dst);
                break;
            }
            default:
                delete dst;
                dst = 0;
                break;
            }

            return NDArrayTyped<T>(*this, dst);
        }
    }

    bool check_inds(QVector<int> &inds);

    QExplicitlySharedDataPointer<ArrayData> sharedData() { return m_data_ptr; }

protected:
    int m_ndim;
    Type m_type;
    QList<int> m_shapelst;
    QSharedPointer<int> m_shape;
    QSharedPointer<int> m_strides;
    int m_size;
    int m_typesize;

    static std::size_t typesize(Type type) {
        switch (type) {
        case Int64: return sizeof(int);
        case Uint8: return sizeof(uint8_t);
        case Float64: return sizeof(double);
        case Float32: return sizeof(float);
        case Bool: return sizeof(bool);
        default: return 0;  // Unknown type
        }
    }

    void update_size();

    template <typename Container>
    int get_ind_temp(Container &inds);

    // Data stuff
    QExplicitlySharedDataPointer<ArrayData> m_data_ptr;
    char* m_data_ch;
};

template <class T>
class NUTMEGLIB_EXPORT NDArrayTyped : public NDArray
{
public:
    explicit NDArrayTyped(const NDArrayTyped<T> &other);
    NDArrayTyped(const NDArray &other, T *dptr=0);
    NDArrayTyped(std::initializer_list<int> shape);
    NDArrayTyped(const QList<int> &shape);

    T &at(const std::initializer_list<int> &inds);
    T &at(const QVector<int> &inds);

    template <typename ...ArgsT>
    T &at(ArgsT ...inds) {
        return at({inds...});
    }

    template <typename ...ArgsT>
    T& operator() (ArgsT ...inds)
    {
        QVector<int> ind({inds...});
        if (!check_inds(ind))
            throw AccessException();
        return at(ind);
    }

    typedef T* iterator;
    typedef const T* const_iterator;
    iterator begin() { return m_data; }
    iterator end() { return m_data + m_size; }
    const_iterator cbegin() const { return m_data; }
    const_iterator cend() const { return m_data + m_size; }

protected:
    T* m_data;
};

Q_DECLARE_METATYPE(NDArray)
Q_DECLARE_METATYPE_TEMPLATE_1ARG(NDArrayTyped)

#endif // NDARRAY_H

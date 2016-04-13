#ifndef ASYNCQUEUE_H
#define ASYNCQUEUE_H

#include <QThread>
#include <QSemaphore>
#include <QQueue>
#include <QDebug>

template <class T>
class AsyncQueue : public QQueue<T>
{
public:
    explicit AsyncQueue()
        : QQueue<T>()
        , m_count(0)
    {
        m_sema = new QSemaphore(1);
//        m_sema.release(1);
        m_sema->acquire(1);
    }

    ~AsyncQueue()
    {
        delete m_sema;
    }

    T dequeue() {
        if (m_count == 0) {
            // Wait for enqueue
            m_sema->acquire(1);
            m_sema->release(1);

        }
        if (m_count == 1) {
            // It's about to be empty, acquire the resource
            m_sema->acquire(1);
        }

        T value = QQueue<T>::dequeue();
        --m_count;
        return value;
    }

    void enqueue(const T& t) {
        QQueue<T>::enqueue(t);
        ++m_count;
        if (m_count == 1) {
            // No longer empty, release the resource
            m_sema->release(1);
        }
    }

    void wait() {
        if (m_sema->available() == 1)
            return;

        m_sema->acquire(1);
        m_sema->release(1);
    }

private:
    QSemaphore *m_sema;
    int m_count;
};

#endif // ASYNCQUEUE_H

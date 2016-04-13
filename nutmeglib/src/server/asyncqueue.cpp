#include "asyncqueue.h"

//template <class T>
//T AsyncQueue<T>::dequeue()
//{
//    if (this->count() == 0) {
//        // Wait for enqueue
//        m_sema->acquire(1);
//        m_sema->release(1);

//    } else if (this->count() == 1) {
//        // It's about to be empty, acquire the resource
//        m_sema->acquire(1);
//    }

//    return QQueue<T>::dequeue();
//}

//template <class T>
//void AsyncQueue<T>::enqueue(const T& t)
//{
//    QQueue<T>::enqueue(t);
//    if (this->count() == 1) {
//        // No longer empty, release the resource
//        m_sema->release(1);
//    }
//}

//template <class T>
//void AsyncQueue<T>::wait() {
//    if (m_sema->available() == 1)
//        return;

//    m_sema->acquire(1);
//    m_sema->release(1);
//}

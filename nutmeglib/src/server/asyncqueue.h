#ifndef ASYNCQUEUE_H
#define ASYNCQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class AsyncQueue
{
public:
  AsyncQueue()
    : m_queue()
    , m_mutex()
    , m_cond()
  {}

  ~AsyncQueue()
  {}

  void enqueue(T t)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(t);
    m_cond.notify_one();
  }

  T dequeue()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_queue.empty())
    {
      m_cond.wait(lock);
    }
    T val = m_queue.front();
    m_queue.pop();
    return val;
  }

private:
  std::queue<T> m_queue;
  mutable std::mutex m_mutex;
  std::condition_variable m_cond;
};

/*
#include <QThread>
#include <QSemaphore>
#include <QQueue>
#include <QDebug>
#include <QAtomicInt>

template <class T>
class AsyncQueue : public QQueue<T>
{
public:
    explicit AsyncQueue()
        : QQueue<T>()
        , m_count(0)
    {
        m_sema = new QSemaphore(1);
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
        --m_count;
        T value = QQueue<T>::dequeue();
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
    QAtomicInt m_count;
};
*/

#endif // ASYNCQUEUE_H

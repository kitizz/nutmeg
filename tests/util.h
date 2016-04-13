#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QElapsedTimer>
#include <QString>
#include <qmath.h>

struct BenchmarkIterator
{
    BenchmarkIterator(QString name)
        : m_name(name)
        , m_timer(QElapsedTimer())
        , m_runCount(0)
        , m_targetCount(2)
        , m_total(0)
    {
        m_timer.start();
    }

    qreal result() {
        return m_total / m_runCount;
    }

    QString report() {
        return QString("Benchmarking %1... Run %2 times: %3 ms").arg(
                    m_name,
                    QString::number(m_runCount),
                    QString::number(result()));
    }

    void next() {
        ++m_runCount;
        if (m_runCount < m_targetCount)
            return;  // Keep going

        if (m_timer.elapsed() < 200) {
            // Too fast, run some more
            m_targetCount *= 2;
        } else {
            // Store the time
            m_total = m_timer.elapsed();
            m_timer.invalidate();
            qDebug() << report();
        }
    }

    bool isDone() {
        return m_total != 0;
    }

private:
    QString m_name;
    QElapsedTimer m_timer;
    int m_runCount;
    int m_targetCount;
    qreal m_total;

};

#define BENCHMARK(NAME) \
    for (BenchmarkIterator __iterator( #NAME ); \
           __iterator.isDone() == false; __iterator.next())

#endif // UTIL_H

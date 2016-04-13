#ifndef SETTINGS_H
#define SETTINGS_H

#include "../nutmeglibdecl.h"
#include <QObject>

class NUTMEGLIB_EXPORT Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int portSubscribe READ portSubscribe WRITE setPortSubscribe NOTIFY portSubscribeChanged)
    Q_PROPERTY(int portPublish READ portPublish WRITE setPortPublish NOTIFY portPublishChanged)

public:
    explicit Settings(QObject *parent = 0);

    int portSubscribe() const;
    int portPublish() const;

signals:
    void portSubscribeChanged(int arg);
    void portPublishChanged(int portPublish);

public slots:
    void setPortSubscribe(int portSubscribe);
    void setPortPublish(int portPublish);

private:
    int m_portSub;
    int m_portPub;
};

#endif // SETTINGS_H

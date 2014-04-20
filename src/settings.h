#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

public:
    explicit Settings(QObject *parent = 0);

    int port() const;
    void setPort(int arg);

signals:
    void portChanged(int arg);

public slots:

private:
    int m_port;
};

#endif // SETTINGS_H

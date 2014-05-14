#ifndef SERVER_H
#define SERVER_H

#include "../nzmqt/nzmqt.hpp"
#include "../mainwindow.h"
#include "pushsocket.h"

#include <QQuickItem>
#include <QByteArray>
#include <QThread>
#include <QQueue>

#include <stdexcept>

using namespace nzmqt;

class MainWindow;
class Server : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(MainWindow* mainWindow READ mainWindow WRITE setMainWindow NOTIFY mainWindowChanged)

public:
    explicit Server(QQuickItem* parent = 0);

    static QString adjustPath(const QString &path);

    // Property accessors
    QString address() const;
    void setAddress(QString arg);
    bool running() const;


    QString updateAddress() const;
    void setUpdateAddress(QString arg);

    int port() const;
    void setPort(int arg);

    MainWindow* mainWindow() const;
    void setMainWindow(MainWindow* arg);

signals:
//    void requestReceived(const QString &request);
    void requestReceived(QVariant request);
    void replySent(const QList<QByteArray>& reply);
    void updateSent(const QList<QByteArray>& update);
    void failure(const QString& tag);
    void finished();
    void createFigure(const QString &handle, const QString &qml);

    // Properties
    void addressChanged(QString arg);
    void runningChanged(bool arg);

    void updateAddressChanged(QString arg);

    void portChanged(int arg);

    void mainWindowChanged(MainWindow* arg);

public slots:
    void processRequest(const QList<QByteArray>& request);
    void sendReply(QVariant reply);
    void parameterChanged(QString figurehandle, QString parameter, QVariant update);
    void parameterUpdated(QString figureHandle, QString parameter);
    void start();
    void stop();
    int getPortForFigure(const QString &figureHandle);

protected:
    static void sleep(unsigned long msecs);
    void setRunning(bool arg);
    void startServer();
    void sendUpdate(QVariant update);
    bool sendPendingUpdate();

private:
    // Allow sleeping
    class ThreadTools : private QThread
    {
    public:
        using QThread::msleep;

    private:
        ThreadTools() {}
    };


    ZMQContext *m_context;
    ZMQSocket* m_socket;
//    QMap<QString, QVariant> m_updateMap;
    QMap<QString, PushSocket*> m_socketUpdateMap;
//    QList<QString> m_updatesToSend;
//    QList<QString> m_updatesToReceive;
    bool m_updateReady;
    int m_latestUpdatePort;

    // Properties
    QString m_address;
    bool m_running;

    QString m_currentAddress;
    int m_port;
    MainWindow* m_mainWindow;
};

#endif // SERVER_H

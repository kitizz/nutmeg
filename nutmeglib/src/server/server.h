#ifndef SERVER_H
#define SERVER_H

#include "../nutmeglibdecl.h"

#include <QQuickItem>
#include <QByteArray>
#include <QThread>
#include <QQueue>
#include <QTime>
#include <QTimer>

#include <stdexcept>

#include "controller.h"

namespace nzmqt {
    class ZMQContext;
    class ZMQSocket;
}

class PushSocket;
class BinarySpec;
class Task;

class NUTMEGLIB_EXPORT Server : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(int portSubscribe READ portSubscribe WRITE setPortSubscribe NOTIFY portSubscribeChanged)
    Q_PROPERTY(int portPublish READ portPublish WRITE setPortPublish NOTIFY portPublishChanged)

    Q_PROPERTY(Controller* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(QObject* app READ app WRITE setApp NOTIFY appChanged)

public:
    explicit Server(QQuickItem* parent = 0);
    ~Server();

    // Property accessors
    QString address() const;
    void setAddress(QString arg);
    bool running() const;

    int portSubscribe() const;
    int portPublish() const;

    Controller *controller() const;
    void setController(Controller *c);

    QObject *app() const;
    void setApp(QObject *app);

signals:
    void serverStarted(int subPort, int pubPort);
    void updateSent(const QList<QByteArray>& update);
    void failure(const QString& tag);
    void finished();
    void createFigure(const QString &handle, const QString &qml);

    // Properties
    void addressChanged(QString arg);
    void runningChanged(bool arg);
    void portSubscribeChanged(int portSubscribe);
    void portPublishChanged(int portPublish);
    void controllerChanged(Controller* controller);
    void appChanged(QObject* app);

public slots:
    // Properties
    void setPortSubscribe(int portSubscribe);
    void setPortPublish(int portPublish);

    // Server functionality
    void processMessage(const QByteArrayList& message);

    void publish(const QString &session, const QVariantMap &msg);
    void pong(const QString &session, int taskId);
    void parameterUpdated(const QString &figurehandle, const QString &parameter, qreal value);
    void success(Task task);
    void error(NutmegError err);
    void requestState(QString session);

    void start();
    void stop();

protected:
    void setRunning(bool arg);
    void startServer();
    void stopSocket(nzmqt::ZMQSocket *socket, const QString &address, int port);
    void sendUpdate(QVariant update);
    bool sendPendingUpdate();

    void updateSession(const QString &session, qint64 timestamp);

    bool findBinding(nzmqt::ZMQSocket *socket, QString &address, int &portSubscribe);

private:
    nzmqt::ZMQContext *m_context;
    nzmqt::ZMQSocket* m_sub;
    nzmqt::ZMQSocket* m_pub;
    bool m_subRunning;
    bool m_pubRunning;
    QMap<QString, PushSocket*> m_socketUpdateMap;
    bool m_updateReady;
    int m_latestUpdatePort;

    QMap<QString, int> m_connectedSessions;

    // Properties
    QString m_address;
    bool m_running;

    QString m_subAddress;
    QString m_pubAddress;
    int m_subPort;
    int m_pubPort;
    Controller* m_controller;
    QObject* m_app;
    QTime m_tim;
    QTimer *m_startTimer;
};

#endif // SERVER_H

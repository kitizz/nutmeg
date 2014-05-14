#ifndef PUSHSOCKET_H
#define PUSHSOCKET_H

#include <QObject>
#include "../nzmqt/nzmqt.hpp"
#include <stdexcept>
#include <QMap>
#include <QTimer>

using namespace nzmqt;

// Unfortunately ZMQSocket doesn't have this info easily accessible.
// And I'm avoiding modifying the ZMQSocket code for the moment...
class PushSocket : public QObject
{
    Q_OBJECT
public:
    PushSocket(ZMQContext *context, int port = 43687, QObject *parent = 0);
    ~PushSocket();

    int bind();
    void unbind();

    bool running() const { return m_running; }
    bool ready() const { return m_ready; }
    int port() const { return m_port; }

    QString address() { return m_host + ":" + QString::number(m_port); }

public slots:
    void receiveMessage(const QList<QByteArray> &msg);
    void sendMessage(const QString &tag, const QList<QByteArray> &msg);
    void sendPendingMessages();
    void replyReceived(const QString &tag);

private slots:
    void checkClient();

signals:
    void close();
//    void messageReceived(const QList<QByteArray> &msg);

private:
    QMap< QString, QList<QByteArray> > m_msgMap;
    QList<QString> m_msgQueue;
    QList<QString> m_msgToReceive;
    ZMQSocket *m_socket;
    QString m_host;
    int m_port;
    bool m_running;
    // If ready is true, the client has sent a request, and is awaiting reply.
    bool m_ready;

    QTimer m_timer;
    bool m_waitingStatus;
};

#endif // PUSHSOCKET_H

#include "pushsocket.h"
#include <QDebug>

PushSocket::PushSocket(ZMQContext *context, int port, QObject *parent)
    : QObject(parent)
    , m_socket(0)
    , m_host("tcp://127.0.0.1")
    , m_port(port)
    , m_running(false)
    , m_ready(false)
    , m_waitingStatus(false)
{
    m_socket = context->createSocket(ZMQSocket::TYP_REP, this);
//    m_socket->setObjectName("RequestSocket_" + figureHandle);
    // Oh lambda functions, how I love thee... <3
    connect(m_socket, &ZMQSocket::messageReceived,
                      this, &PushSocket::receiveMessage);

    connect(&m_timer, &QTimer::timeout, this, &PushSocket::checkClient);
    m_timer.setSingleShot(true);
    m_timer.start(5000);
}

PushSocket::~PushSocket()
{
    delete m_socket;
}

/*!
 * \brief PushSocket::bind
 * Bind to the first available port number starting with the current port.
 * \return
 */
int PushSocket::bind()
{
    if (m_running)
        return m_port;

    int findingPort = 100;
    while (findingPort && !m_running) {
        try {
            m_ready = false;
            m_socket->bindTo(address());
            m_running = true;
            m_waitingStatus = false;
            qDebug() << "PushSocket connected to port" << m_port;
            return m_port;
        }
        catch (const nzmqt::ZMQException& ex) {
            if (ex.num() == 48) {
                // Unable to bind to address...
                findingPort--;
                m_port++;
                if (m_port > 65535) m_port = 40000;
            } else {
                qWarning() << Q_FUNC_INFO << "Exception:" << ex.num() << ex.what();
                findingPort = 0;
            }
        }
    }

    return -1;
}

void PushSocket::unbind()
{
    if (m_ready) {
        QList<QByteArray> msg;
        msg << QString("closing").toLocal8Bit();
        m_socket->sendMessage(msg);
        m_ready = false;
    }
    if (m_running) {
        try {
            m_socket->unbindFrom(address());
        }
        catch (const nzmqt::ZMQException& ex) {
            qWarning() << Q_FUNC_INFO << "Exception:" << ex.what();
        }
        m_running = false;
    }
}

void PushSocket::receiveMessage(const QList<QByteArray> &msg)
{

    QString command(msg[0]);

    if (command == "ready") {
        m_waitingStatus = false;
        m_ready = true;
        m_timer.start(5000);
        m_msgToReceive.removeOne("status");
        sendPendingMessages();
    } else if (command == "closing") {
        emit close();
    }
//    emit messageReceived(msg);
}

void PushSocket::sendMessage(const QString &tag, const QList<QByteArray> &msg)
{
    if (!m_running) {
        qWarning() << Q_FUNC_INFO << "Socket is not running.";
        return;
    }
    qDebug() << Q_FUNC_INFO << "Tag:" << tag << "Msg:" << msg;
    if (!m_msgMap.contains(tag))
        m_msgQueue.append(tag);
    m_msgMap.insert(tag, msg);
    sendPendingMessages();
}

void PushSocket::sendPendingMessages()
{
    qDebug() << "PushSocket ready:" << m_ready;
    if (!m_ready) return;
    if (m_msgQueue.length() == 0) return;

    for (int i=0; i<m_msgQueue.length(); ++i) {
        QString key = m_msgQueue[i];
//        // TO DO: Hmmmm, how to deal with the fact that requests come back on the server's main socket...
//        if (m_msgToReceive.contains(key))
//            continue;
        m_ready = false;
        m_socket->sendMessage(m_msgMap[key]);
        m_msgMap.remove(key);
        m_msgQueue.takeAt(i);
        // Status messages don't require replies
//        if (tag != "status")
        m_msgToReceive.append(key);
        return;
    }
}

void PushSocket::replyReceived(const QString &tag)
{
    qDebug() << "Update received for" << tag;
    m_msgToReceive.removeOne(tag);
}

void PushSocket::checkClient()
{
    if (m_waitingStatus) {
        qDebug() << "Client unresponsive... Ping timeout.";
//        unbind();
        return;
    }
    QList<QByteArray> msg;
    msg << QString("[\"ping\"]").toLocal8Bit();
    this->sendMessage("status", msg);
    m_waitingStatus = true;
    m_timer.start(3000);
}



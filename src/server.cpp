#include "server.h"

#include <QDateTime>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QCoreApplication>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDir>

Server::Server(QQuickItem *parent)
    : QQuickItem(parent)
    , m_socket(0)
    , m_updateSocket(0)
    , m_updateMap(QMap<QString, QVariant>())
    , m_address(QString("tcp://127.0.0.1"))
    , m_updateAddress(QString())
    , m_updatesToSend(QList<QString>())
    , m_updatesToReceive(QList<QString>())
    , m_updateReady(false)
    , m_running(false)
    , m_port(43686) // gemt'N <-> N'tmeg
{
    ZMQContext *context = createDefaultContext(this);
    context->start();

    m_socket = context->createSocket(ZMQSocket::TYP_REP, this);
    m_socket->setObjectName("Replier.Socket.socket(REP)");
    connect(m_socket,   SIGNAL(messageReceived(const QList<QByteArray>&)),
                        SLOT(processRequest(const QList<QByteArray>&)));

    m_updateSocket = context->createSocket(ZMQSocket::TYP_REP, this);
    m_updateSocket->setObjectName("Replier.Socket.socket(REP)");
    connect(m_updateSocket,   SIGNAL(messageReceived(const QList<QByteArray>&)),
                        SLOT(updateRequest(const QList<QByteArray>&)));
}


QString Server::address() const
{
    return m_address;
}

void Server::setAddress(QString arg)
{
    if (m_address == arg) return;

    bool wasRunning = m_running;
    if (m_running) this->stop();

    m_address = arg;
    emit addressChanged(arg);

    if (wasRunning) this->start();
}

bool Server::running() const
{
    return m_running;
}

QString Server::updateAddress() const
{
    return m_updateAddress;
}

void Server::setUpdateAddress(QString arg)
{
    if (m_updateAddress == arg) return;
    m_updateAddress = arg;
    emit updateAddressChanged(arg);
}

int Server::port() const
{
    return m_port;
}

void Server::setPort(int arg)
{
    if (m_port == arg) return;
    m_port = arg;
    emit portChanged(arg);
    stop();
    start();
}

void Server::setRunning(bool arg)
{
    if (m_running == arg) return;
    m_running = arg;
    emit runningChanged(arg);
}

void Server::processRequest(const QList<QByteArray> &request)
{
    static quint64 counter = 0;
    counter++;
//    qDebug() << "Replier::requestReceived> " << counter << request;

    QJsonDocument doc = QJsonDocument::fromJson(request[0]);
    emit requestReceived(doc.toVariant());

}

void Server::updateRequest(const QList<QByteArray> &request)
{
    static quint64 updateCounter = 0;
    updateCounter++;

    QString command(request[0]);

    if (command == "ready") {
        if (!sendPendingUpdate())
            m_updateReady = true;
    }
}

void Server::sendReply(QVariant reply)
{
    QList<QByteArray> byteReply;
    byteReply << reply.toString().toLocal8Bit();
    m_socket->sendMessage(byteReply);

    emit replySent(byteReply);
}

void Server::parameterChanged(QString figureHandle, QString parameter, QVariant update)
{
    if (m_updateReady) {
        m_updateReady = false;
        sendUpdate(update);
    } else {
        QString key = figureHandle + "." + parameter;
        if (!m_updateMap.contains(key)) {
            // updateMap and updatesToSend should stay synced
            m_updatesToSend.append(key);
        }
        m_updateMap.insert(key, update);
    }
}

void Server::parameterUpdated(QString figureHandle, QString parameter)
{
    QString key = figureHandle + "." + parameter;
    if (m_updatesToReceive.contains(key)) {
        m_updatesToReceive.removeOne(key);
        if (m_updateReady)
            sendPendingUpdate();
    }
}

void Server::start()
{
    try {
        startServer();
        setRunning(true);
    }
    catch (const nzmqt::ZMQException& ex) {
        qWarning() << Q_FUNC_INFO << "Exception:" << ex.what();
        emit failure(ex.what());
        emit finished();
    }
}

void Server::stop()
{
    try {
        m_socket->unbindFrom(m_currentAddress);
    }
    catch (const nzmqt::ZMQException& ex) {
        qWarning() << Q_FUNC_INFO << "Exception:" << ex.what();
        emit failure(ex.what());
    }
    setRunning(false);
    emit finished();
}

void Server::sleep(unsigned long msecs)
{
    ThreadTools::msleep(msecs);
}

void Server::startServer()
{
    m_currentAddress = m_address + ":" + QString::number(m_port);
    m_socket->bindTo(m_currentAddress);
    m_updateSocket->bindTo(m_updateAddress);
}

void Server::sendUpdate(QVariant update)
{
    QList<QByteArray> byteReply;
    byteReply << update.toString().toLocal8Bit();
    m_updateSocket->sendMessage(byteReply);

    emit updateSent(byteReply);
}

bool Server::sendPendingUpdate()
{
    if (!m_updateReady) return false;
    if (m_updatesToSend.length() == 0) return false;

    m_updateReady = false;
    for (int i=0; i<m_updatesToSend.length(); ++i) {
        QString key = m_updatesToSend[i];
        if (m_updatesToReceive.contains(key))
            continue;
        sendUpdate(m_updateMap[key]);
        m_updateMap.remove(key);
        m_updatesToReceive.append(m_updatesToSend.takeAt(i));
        return true;
    }
    return false;
}

QString Server::adjustPath(const QString &path)
{
#if defined(Q_OS_IOS)
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("%1/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#elif defined(Q_OS_MAC)
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("%1/../Resources/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#elif defined(Q_OS_BLACKBERRY)
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("app/native/%1").arg(path);
#elif !defined(Q_OS_ANDROID)
    QString pathInInstallDir =
            QString::fromLatin1("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
    pathInInstallDir =
            QString::fromLatin1("%1/%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#elif defined(Q_OS_ANDROID_NO_SDK)
    return QLatin1String("/data/user/qt/") + path;
#endif
    return path;
}

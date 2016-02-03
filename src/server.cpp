#include "server.h"

#include <QDateTime>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QList>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDir>

Server::Server(QQuickItem *parent)
    : QQuickItem(parent)
    , m_socket(0)
    , m_socketUpdateMap(QMap<QString, PushSocket*>())
    , m_updateReady(false)
    , m_latestUpdatePort(43687)
    , m_address(QString("tcp://0.0.0.0"))
    , m_running(false)
    , m_port(43686) // gemt'N <-> N'tmeg
    , m_app(0)
{
    m_context = createDefaultContext(this);
    m_context->start();

    m_socket = m_context->createSocket(ZMQSocket::TYP_REP, this);
    m_socket->setObjectName("Replier.Socket.socket(REP)");
    connect(m_socket,   SIGNAL(messageReceived(const QList<QByteArray>&)),
                        SLOT(processRequest(const QList<QByteArray>&)));
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

int Server::port() const
{
    return m_port;
}

void Server::setPort(int arg)
{
    if (m_port == arg) return;
    m_port = arg;
    emit portChanged(arg);
    if (m_running) {
        stop();
        start();
    }
}

MainWindow *Server::mainWindow() const
{
    return m_mainWindow;
}

void Server::setMainWindow(MainWindow *arg)
{
    if (m_mainWindow == arg) return;
    m_mainWindow = arg;
    emit mainWindowChanged(arg);
}

QObject *Server::app() const
{
    return m_app;
}

void Server::setApp(QObject *app)
{
    if (m_app == app)
        return;

    m_app = app;
    emit appChanged(app);
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
    qDebug() << "Replier::requestReceived> " << counter;

    if (request.count() < 2) {
        qWarning() << "WARNING: Request must be at least length 2! Received:" << request;
        return;
    }

    QString msgType = QString(request[0]);
    QJsonDocument doc = QJsonDocument::fromJson(request[1]);
    if (msgType == "json") {
        // [msgType, msg]
        // Msg: {"handle": h, "data": {props: values}, "parameter": param}
        emit jsonReceived(doc.toVariant());

    } else if (msgType == "binary") {
        // [msgType, msg, binaryData]
        // Msg: {"handle": h, "property": prop, "parameter": param}
        emit binaryReceived(doc.toVariant(), QVariant(request[2]));

    } else {
        // Unknown message type
    }

}

void Server::sendReply(QVariant reply)
{
    QList<QByteArray> byteReply;
    byteReply << reply.toString().toLocal8Bit();
    m_socket->sendMessage(byteReply);
    qDebug() << "Replier::replySent ";

    emit replySent(byteReply);
}

void Server::parameterChanged(QString figureHandle, QString parameter, QVariant update)
{
    QList<QByteArray> msg;
    msg << update.toString().toLocal8Bit();

    PushSocket *socket = m_socketUpdateMap.value(figureHandle, 0);
    if (socket) {
        qDebug() << "socket for" << figureHandle;
        QString tag = figureHandle + "." + parameter;
        socket->sendMessage(tag, msg);
    } else {
        qDebug() << "No socket for" << figureHandle;
    }
}

void Server::parameterUpdated(QString figureHandle, QString parameter)
{
    qDebug() << Q_FUNC_INFO << figureHandle << parameter;
    if (!m_socketUpdateMap.contains(figureHandle)) return;

    PushSocket *socket = m_socketUpdateMap[figureHandle];
    QString tag = figureHandle + "." + parameter;
    socket->replyReceived(tag);
}

void Server::start()
{
    int findingPort = 50;
    while (findingPort) {
        try {
            startServer();
            setRunning(true);
            findingPort = 0;
            qDebug() << "Connected to port" << m_port;
            if (m_mainWindow) {
                m_mainWindow->notify("Server Started", "Port set to " + QString::number(m_port));
            }
        }
        catch (const nzmqt::ZMQException& ex) {
            if (ex.num() == 48) {
                findingPort--;
                setPort(m_port + 1);
            } else {
                qWarning() << Q_FUNC_INFO << "Exception:" << ex.num() << ex.what();
                findingPort = 0;
                emit failure(ex.what());
                emit finished();
            }
        }
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

int Server::getPortForFigure(const QString &figureHandle)
{
    PushSocket *socket;
    if (m_socketUpdateMap.contains(figureHandle)) {
        socket = m_socketUpdateMap[figureHandle];

    } else {
        socket = new PushSocket(m_context, m_latestUpdatePort + 1);
        connect(socket, &PushSocket::close,
                [=](){
            m_socketUpdateMap.remove(figureHandle);
            delete socket;
        });
        // Add it to the map
        m_socketUpdateMap.insert(figureHandle, socket);
    }

    socket->unbind(); // Ensures that client is informed of closure.
    m_latestUpdatePort = socket->bind();

    return socket->port();
}

void Server::sleep(unsigned long msecs)
{
    ThreadTools::msleep(msecs);
}

void Server::startServer()
{
    m_currentAddress = m_address + ":" + QString::number(m_port);
    m_socket->bindTo(m_currentAddress);
//    m_updateSocket->bindTo(m_updateAddress);
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

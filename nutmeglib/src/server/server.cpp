#include "server.h"

#include <QDateTime>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QCoreApplication>
#include <QList>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArrayList>

#include "binaryspec.h"
#include "task.h"
#include "pushsocket.h"
#include "server_util.h"
#include "../util/ndarray.h"

#include "../3rdparty/nzmqt/nzmqt.hpp"
using namespace nzmqt;

Server::Server(QQuickItem *parent)
    : QQuickItem(parent)
    , m_sub(0)
    , m_pub(0)
    , m_subRunning(false)
    , m_pubRunning(false)
    , m_socketUpdateMap(QMap<QString, PushSocket*>())
    , m_updateReady(false)
    , m_latestUpdatePort(43687)
    , m_connectedSessions(QMap<QString, int>())
    , m_address(QString("tcp://0.0.0.0"))
    , m_running(false)
    , m_subAddress(m_address)
    , m_pubAddress(m_address)
    , m_subPort(43686) // gemt'N <-> N'tmeg
    , m_pubPort(m_subPort + 1)
    , m_controller(0)
    , m_app(0)
    , m_tim(QTime())
    , m_startTimer(new QTimer())
{
    // Once the server is started, request a full state update from anything connected
    m_startTimer->setSingleShot(true);
    connect(m_startTimer, &QTimer::timeout, [=]() { requestState("Nutmeg"); });

    m_tim.start();

    m_context = createDefaultContext(this);
    m_context->start();

    // Create and establish Subscribe Socket
    m_sub = m_context->createSocket(ZMQSocket::TYP_SUB, this);
    m_sub->setObjectName("Subscriber.Socket.socket(Sub)");
    connect(m_sub,   SIGNAL(messageReceived(const QList<QByteArray>&)),
                        SLOT(processMessage(const QList<QByteArray>&)));
    m_sub->subscribeTo("Nutmeg");

    // Publish socket
    m_pub = m_context->createSocket(ZMQSocket::TYP_PUB, this);
    m_pub->setObjectName("Subscriber.Socket.socket(Pub)");
}

Server::~Server()
{
    m_sub->close();
    m_pub->close();
    m_context->stop();

    m_sub->deleteLater();
    m_pub->deleteLater();
    m_context->deleteLater();

    m_startTimer->deleteLater();
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
    m_subAddress = m_address;
    m_pubAddress = m_address;
    emit addressChanged(arg);

    if (wasRunning) this->start();
}

bool Server::running() const
{
    return m_running;
}

int Server::portSubscribe() const
{
    return m_subPort;
}

void Server::setPortSubscribe(int portSubscribe)
{
    if (m_subPort == portSubscribe)
        return;

    if (m_subRunning) {
        stopSocket(m_sub, m_subAddress, m_subPort);
        m_subRunning = false;
    }

    m_subPort = portSubscribe;
    m_subRunning = findBinding(m_sub, m_subAddress, m_subPort);

    emit portSubscribeChanged(m_subPort);

    if (!m_subRunning && running()) {
        // If binding failed, and the server was previously running, set it as no longer running
        setRunning(false);
        emit finished();
    } else if (running()) {
        emit serverStarted(m_subPort, m_pubPort);
    }
}

int Server::portPublish() const
{
    return m_pubPort;
}

void Server::setPortPublish(int portPublish)
{
    if (m_pubPort == portPublish)
        return;


    if (m_pubRunning) {
        stopSocket(m_pub, m_pubAddress, m_pubPort);
        m_pubRunning = false;
    }

    m_pubPort = portPublish;
    m_pubRunning = findBinding(m_pub, m_pubAddress, m_pubPort);

    emit portPublishChanged(m_pubPort);

    if (!m_pubRunning && running()) {
        setRunning(false);
        emit finished();
    } else if (running()) {
        emit serverStarted(m_subPort, m_pubPort);
    }
}

Controller *Server::controller() const
{
    return m_controller;
}

void Server::setController(Controller *c)
{
    if (m_controller == c)
        return;

    if (m_controller)
        m_controller->disconnect(this);

    m_controller = c;

    if (m_controller) {
        connect(m_controller, &Controller::parameterUpdated, this, &Server::parameterUpdated);
        connect(m_controller, &Controller::errorProcessing, this, &Server::error);
        connect(m_controller, &Controller::successProcessing, this, &Server::success);
    }

    emit controllerChanged(c);
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

void Server::processMessage(const QByteArrayList &message)
{
    qint64 timestamp = m_tim.elapsed();

    static quint64 counter = 0;
    qDebug() << "Subscriber::messageReceived> " << counter;
    ++counter;

    // message[0] == "Nutmeg"
    if (message.count() < 3) {
        qWarning() << "WARNING: Request must be at least length 2! Received:" << message;
        return;
    }

    qDebug() << message[1].constData() << "\n";
    QJsonDocument doc = QJsonDocument::fromJson(message[1]);
    if (!doc.isObject()) {
        qWarning() << "Message[0] is not a JSON object...";
        return;
    }

    QByteArrayList data = message.mid(2);

    QJsonObject msg = doc.object();
    QString command = msg["command"].toString();
    if (command.isEmpty()) {
        qWarning() << "Message does not define a \"command\": str";
        return;
    }

    QString target = msg["target"].toString();

    QVariantList args = msg["args"].toArray().toVariantList();

    QString session_str = msg["session"].toString();
    QUuid session(session_str);
    updateSession(session_str, timestamp);

    uint64_t id = uint64_t( msg["id"].toInt() );

    if (command == "Ping") {
        // Handle Pings specially
        pong(session_str, id);

    } else if (m_controller) {
        QJsonArray bin_arr = msg["binary"].toArray();
        QList<BinarySpec> bin_specs;
        foreach (QJsonValue bin_v, bin_arr) {
            bin_specs << BinarySpec(bin_v.toObject());
        }

        // Sub in binary values where needed
        ServerUtil::parseBinary(args, bin_specs, data);
        m_controller->queueTask(command, target, args, session, id);
    }
}

void Server::publish(const QString &session, const QVariantMap &data)
{
    if (!m_pubRunning)
        return;

    QList<QByteArray> msg;

    msg << session.toLatin1();

    QJsonDocument doc(QJsonObject::fromVariantMap(data));
    msg << doc.toJson();

    qDebug() << "Publishing message:" << msg[1].constData();

    m_pub->sendMessage(msg);
}

void Server::pong(const QString &session, int taskId)
{
    QVariantMap msg;

    msg["messageType"] = QString("success");
    msg["session"] = session;
    msg["id"] = taskId;

    publish(session, msg);
}

void Server::parameterUpdated(const QString &figureHandle, const QString &parameter, qreal value)
{
    QVariantMap msg;

    msg["messageType"] = QString("parameterUpdated");

    msg["figureHandle"] = figureHandle;
    msg["parameter"] = parameter;
    msg["value"] = value;

    qDebug() << "Parameter updated. Sending:";
    qDebug() << msg;
    publish("Nutmeg", msg);
}

void Server::success(Task task)
{
    QVariantMap msg;
    QString session = task.sessionId.toString();

    msg["messageType"] = QString("success");
    msg["session"] = session;
    msg["id"] = task.taskId;

    publish(session, msg);
}

void Server::error(NutmegError err)
{
    Task &task = err.task;
    QString session = task.sessionId.toString();
    QVariantMap msg;

    msg["messageType"] = QString("error");
    msg["session"] = session;
    msg["id"] = task.taskId;

    msg["errorName"] = err.name;
    msg["message"] = err.message;
    msg["details"] = err.details;

//    qWarning() << "\nError in Nutmeg!";
    QByteArray details = QJsonDocument(QJsonObject::fromVariantMap(err.details)).toJson();
    qWarning("\n%s: %s", qPrintable(err.name), qPrintable(err.message));
    qWarning() << "Details:" << qPrintable(details);
    qWarning() << "Session:" << task.sessionId;
    qWarning() << "Task ID:" << task.taskId << "\n";

    publish(session, msg);
}

void Server::requestState(QString session)
{
    QVariantMap msg;

    msg["messageType"] = QString("requestState");
//    msg["session"] = sessions;

    publish(session, msg);
}

void Server::start()
{
    startServer();
}

void Server::stopSocket(ZMQSocket *socket, const QString &address, int port)
{
    QString fullAddress = QString("%1:%2").arg(address).arg(port);
    try {
        socket->unbindFrom(fullAddress);
    } catch (const nzmqt::ZMQException& ex) {
        qWarning() << "Did not unbind from:" << fullAddress;
        qWarning() << Q_FUNC_INFO << "Exception:" << ex.num() << ex.what();
        emit failure(ex.what());
    }
}

void Server::updateSession(const QString &session, qint64 timestamp)
{
    if (!m_connectedSessions.contains(session)) {
        // If this is a previously unknown session, request its state
        requestState(session);
    }
    m_connectedSessions[session] = timestamp;
}

void Server::stop()
{
    stopSocket(m_sub, m_subAddress, m_subPort);
    stopSocket(m_pub, m_pubAddress, m_pubPort);

    setRunning(false);
    emit finished();
}

void Server::startServer()
{
    int subPort = m_subPort;
    m_subRunning = findBinding(m_sub, m_subAddress, subPort);

    int pubPort = subPort + 1;
    m_pubRunning = findBinding(m_pub, m_pubAddress, pubPort);

    if (m_subRunning)
        setPortSubscribe(subPort);
    if (m_pubRunning)
        setPortPublish(pubPort);

    if (m_subRunning && m_pubRunning) {
        setRunning(true);
        emit serverStarted(m_subPort, m_pubPort);
        m_startTimer->start(100);
    } else {
        emit finished();
    }
}

bool Server::findBinding(ZMQSocket *socket, QString &address, int &port)
{
    QString temp("%1:%2");
    for (int i=0; i<100; ++i) {
        QString fullAddress = temp.arg(address).arg(port + i);
        try {
            socket->bindTo(fullAddress);
            return true;
        } catch (const ZMQException &ex) {
            if (ex.num() == 48)
                continue;

            qCritical() << Q_FUNC_INFO << "Exception:" << ex.num() << ex.what();
            qCritical() << "Address provided:" << fullAddress;
            emit failure(ex.what());
            emit finished();
        }
    }

    return false;
}

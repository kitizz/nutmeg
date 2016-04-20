#include "qmlwindow.h"

#include <QVBoxLayout>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

QmlWindow::QmlWindow(QUrl qmlSource, bool persistent, QWidget *parent, bool delayViewInit)
    : QWidget(parent)
    , m_view(0)
    , m_source(qmlSource)
    , m_lastSize(QSize())
    , m_exiting(false)
    , m_persistent(persistent)
{
    m_menuBar = new QMenuBar(0);

    m_view = new QQuickView();
    m_view->connect(m_view->engine(), &QQmlEngine::quit, m_view, &QWindow::close);

    // TODO: Remove this hack. Look at this post:
    // http://www.qtcentre.org/threads/43236-Loading-a-qmldir-from-a-qrc-file?p=198993#post198993

    QString libpath = adjustPath("");
    qDebug() << "Lib path:" << libpath;
    m_view->engine()->addImportPath(libpath);

    if (!delayViewInit)
        finalizeView();
}

void QmlWindow::finalizeView()
{
    QWidget *container = QWidget::createWindowContainer(m_view);
    container->setMinimumSize(100, 100);
    container->setMaximumSize(60000, 60000);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    m_view->setSource(m_source);
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
#ifdef Q_OS_WIN
    layout->addWidget(m_menuBar);
#endif
    layout->addWidget(container);
}

QMenuBar *QmlWindow::menuBar()
{
    return m_menuBar;
}

bool QmlWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Close && !m_exiting && m_persistent) {
        qDebug() << "Event exit";
        showHide();
        event->setAccepted(false);
        return false;
    }
    return QWidget::event(event);
}

QString QmlWindow::adjustPath(const QString &path)
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


void QmlWindow::showHide()
{
    if (isVisible()) {
        m_lastSize = this->size();
        hide();
    } else {
        show();
        this->resize(m_lastSize);
        raise();
    }
}

void QmlWindow::exit()
{
    m_exiting = true;
    if (m_view) {
        delete m_view;
        m_view = 0;
    }
    this->close();
}

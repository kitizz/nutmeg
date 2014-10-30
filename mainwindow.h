#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qmlwindow.h"
#include <QQuickView>
#include <QSystemTrayIcon>
#include "src/server.h"
#include <QFileDialog>
#include <QJSValue>

class Server;
class MainWindow : public QmlWindow
{
    Q_OBJECT
    Q_PROPERTY(Server* server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(QmlWindow* settingsWindow READ settingsWindow WRITE setSettingsWindow NOTIFY settingsWindowChanged)
    Q_PROPERTY(QmlWindow* aboutWindow READ aboutWindow WRITE setAboutWindow NOTIFY aboutWindowChanged)

public:
    explicit MainWindow(QUrl qmlSource, QWidget *parent = 0);

    Server* server() const;
    void setServer(Server* arg);

    QmlWindow* settingsWindow() const;
    void setSettingsWindow(QmlWindow *arg);

    QmlWindow* aboutWindow() const;
    void setAboutWindow(QmlWindow* arg);

signals:
    void serverChanged(Server* arg);
    void settingsWindowChanged(QmlWindow* arg);

    void aboutWindowChanged(QmlWindow* arg);
    void fileSelected(QString file);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showSettings();
    void showAbout();

    void saveFigure();
    void loadFigure();
    void savePdfDialog(QString filename);
//    void savePdf();

public slots:
    void notify(const QString &title, const QString &msg);

private:
    void createSystemTray();
    void createMenus();
    QSystemTrayIcon *m_trayIcon;

    Server* m_server;
    QmlWindow* m_settingsWindow;
    QmlWindow* m_aboutWindow;

    static QFileDialog *m_dialog;
};

#endif // MAINWINDOW_H

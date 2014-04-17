#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QQuickView>
#include <QSystemTrayIcon>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QUrl qmlSource, QWidget *parent = 0);
    QQuickView *view;

signals:

protected:
    virtual bool event(QEvent *event);

protected slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHide();
    void onExit();

public slots:

private:
    void createSystemTray();
    QSystemTrayIcon *m_trayIcon;

    QSize m_lastSize;
    bool m_exiting;

};

#endif // MAINWINDOW_H

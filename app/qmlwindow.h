#ifndef QMLWINDOW_H
#define QMLWINDOW_H

#include <QWidget>
#include <QMenuBar>
#include <QQuickView>

class QmlWindow : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QMenuBar *menuBar READ menuBar NOTIFY menuBarChanged)
public:
    explicit QmlWindow(QUrl qmlSource, bool persistent = false, QWidget *parent = 0, bool delayViewInit = false);
    QMenuBar *menuBar();
    QQuickView *view() const { return m_view; }

    virtual bool event(QEvent *event);

signals:

    void menuBarChanged(QMenuBar * arg);

public slots:
    void exit();
    void showHide();

protected:
    void finalizeView();

private:
    QMenuBar *m_menuBar;
    QQuickView *m_view;
    QUrl m_source;
    QSize m_lastSize;
    bool m_exiting;
    bool m_persistent;

};

#endif // SETTINGSWINDOW_H

#ifndef QMLWINDOW_H
#define QMLWINDOW_H

#include <QWidget>
#include <QQuickView>

class QmlWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QmlWindow(QUrl qmlSource, bool persistent = false, QWidget *parent = 0);
    QQuickView *view() const { return m_view; }

    virtual bool event(QEvent *event);

signals:

public slots:
    void exit();
    void showHide();

private:
    QQuickView *m_view;
    QSize m_lastSize;
    bool m_exiting;
    bool m_persistent;

};

#endif // SETTINGSWINDOW_H

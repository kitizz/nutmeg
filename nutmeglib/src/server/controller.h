#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QQuickItem>
#include <QThread>

#include "task.h"
#include "asyncqueue.h"
#include "../common/figurebase.h"
#include "nutmegerror.h"

class Controller;
class ControllerWorker : public QThread
{
Q_OBJECT
public:
    ControllerWorker(Controller *c, AsyncQueue<Task> *queue)
        : m_controller(c)
        , m_taskqueue(queue)
    {}

signals:
    void createFigure(const Task &task);

private:
    void run() Q_DECL_OVERRIDE;
    void setFigure(Task *task);
    void setGui(Task *task);
    void setProperty(Task *task, bool inGui=false);
    void invoke(Task *task);

    Controller *m_controller;
    AsyncQueue<Task> *m_taskqueue;
};

class FigureBase;
class NUTMEGLIB_EXPORT Controller : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap figures READ figures NOTIFY figuresChanged)
    Q_PROPERTY(QQuickItem *figureContainer READ figureContainer WRITE setFigureContainer NOTIFY figureContainerChanged)
    Q_PROPERTY(QQuickItem *guiContainer READ guiContainer WRITE setGuiContainer NOTIFY guiContainerChanged)

public:
    explicit Controller(QQuickItem *parent = 0);
    ~Controller();

    FigureBase *figure(const QString &handle);
    QVariantMap figures() const;
//    QList<FigureBase*> getFiguresByHandle(const QString &handle);

    NutmegObject *findObject(const QStringList &parts, Task *task, bool inGui=false);
    QMetaProperty findProperty(Task *task, bool inGui=false);
    QMetaMethod findMethod(Task *task, bool inGui=false);

    QQuickItem *createQmlObject(const QByteArray &qml, const QString &name, QQuickItem *parent, const Task &task);

    QQuickItem *figureContainer() const;
    void setFigureContainer(QQuickItem *figureContainer);

    QQuickItem *guiContainer() const;
    void setGuiContainer(QQuickItem *guiContainer);

signals:
    void errorProcessing(NutmegError error);
    void parameterUpdated(QString figureHandle, QString parameter, qreal value);

    void figureCreated(FigureBase* figure);
    void figureDestroyed(FigureBase* figure);

    void figuresChanged(QVariantMap arg);
    void figureContainerChanged(QQuickItem *figureContainer);
    void guiContainerChanged(QQuickItem * guiContainer);

public slots:
    void deregisterFigure(FigureBase *fig);
    void registerFigure(FigureBase *fig);
    void queueTask(const QString &command, const QString &target, const QVariantList &args, const QUuid &session, uint64_t id);
    void queueTask(const QString &command, const QString &target, const QVariantList &args, int id);
    void queueTask(const Task &task);

    void createFigure(Task task);
    void createGui(FigureBase *fig, Task task);

protected slots:
//    void createFigure(const Task& task);
    void updateFigures();

private:
    bool m_destroying;
    QMultiMap<QString,FigureBase*> m_figures;
    QVariantMap m_figuresVar;

    ControllerWorker *m_worker;
    AsyncQueue<Task> m_taskqueue;

    QQuickItem *m_figureContainer;
    QQuickItem *m_guiContainer;
};

#endif // CONTROLLER_H

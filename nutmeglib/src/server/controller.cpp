#include "controller.h"

#include <QQueue>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>
#include <QMetaMethod>

#include "server_util.h"
#include "../common/guiitem.h"
#include "../util/ndarray.h"

/*!
 * \brief ControllerWorker::run
 * \throws UnknownCommand
 */
void ControllerWorker::run()
{
    if (!m_taskqueue)
        return;

    while (true) {
        Task task = m_taskqueue->dequeue();

        // Note on success reporting: SetFigure and SetGui invoke methods on the main thread to
        // complete the task. So allow them to report their own success
        try {
            switch (task.command) {
            case Task::SetFigure:
                setFigure(&task);
                break;

            case Task::SetProperty:
                setProperty(&task, false);
                emit m_controller->successProcessing(task);
                break;

            case Task::Invoke:
                invoke(&task);
                emit m_controller->successProcessing(task);
                break;

            case Task::SetGui:
                setGui(&task);
                break;

            case Task::SetGuiProperty:
                setProperty(&task, true);
                emit m_controller->successProcessing(task);
                break;

            case Task::AddComponent:
                addComponent(&task);
                break;

            default:
                throw UnknownCommand(task);
                break;
            }

        } catch (NutmegError &err) {
            emit m_controller->errorProcessing(err);
        }
    }
}

/*!
 * \brief ControllerWorker::addComponent
 * \param task
 * \throws QmlParserError
 */
void ControllerWorker::addComponent(Task *task)
{
    QStringList parts = task->target.split('.');
    if (parts.length() != 1) {
        throw InvalidTarget(*task, QString("Invalid Component handle, \"%1\". Avoid dot representation").arg(task->target));
    }

    if (task->args.length() != 1)
        throw BadArgumentCount(*task, 1, task->args.length());

    QByteArray qml = task->args[0].toByteArray();

    // TODO: Check if Component is already exists

    // Call controller queued
    QMetaObject::invokeMethod(m_controller, "createComponent", Qt::BlockingQueuedConnection,
                              Q_ARG(Task, *task));
}

/*!
 * \brief ControllerWorker::setFigure
 *
 * Set a Figure defined by a QML. An example message spec:
 * Part 1:
 * ```
 * {
 *     "command": "SetFigure",
 *     "target": "figureHandle",
 *     "args": [ <qml definition> ]
 * }
 * ```
 *
 * \throws QmlParserError, FigureError
 */
void ControllerWorker::setFigure(Task *task)
{
    QStringList parts = task->target.split('.');
    if (parts.length() != 1) {
        throw InvalidTarget(*task, QString("Invalid Figure handle, \"%1\". Figure handle is always the root object.").arg(task->target));
    }

    if (task->args.length() != 1)
        throw BadArgumentCount(*task, 1, task->args.length());

    QByteArray qml = task->args[0].toByteArray();

    FigureBase *fig = m_controller->figure(parts[0]);
    if (fig) {
        if (fig->qml() == qml)
            return; // We redefine it?

        m_controller->deregisterFigure(fig);
//        fig->deleteLater();
    }

    // Call controller queued
    QMetaObject::invokeMethod(m_controller, "createFigure", Qt::BlockingQueuedConnection,
                              Q_ARG(Task, *task));
}

/*!
 * \brief ControllerWorker::setGui
 * \param task - Task#
 * \throws InvalidTarget, BadArgumentCount, QmlParserError
 */
void ControllerWorker::setGui(Task *task)
{
    QStringList parts = task->target.split('.');
    if (parts.length() != 1) {
        throw InvalidTarget(*task, QString("Invalid Figure handle, \"%1\". Figure handle is always the root object.").arg(task->target));
    }

    if (task->args.length() != 1)
        throw BadArgumentCount(*task, 1, task->args.length());

    FigureBase *fig = m_controller->figure(parts[0]);
    if (!fig)
        throw InvalidTarget(*task, QString("No Figure found with handle, \"%1\".").arg(task->target));

    QByteArray qml = task->args[0].toByteArray();
    if (qml.isEmpty())
        return;  // No GUI defined, but no need to throw an error

    GuiBase *gui = fig->gui();
    if (gui) {
        connect(gui, &GuiBase::parameterChanged, m_controller, &Controller::parameterUpdated);
        gui->deleteLater();
    }

    QMetaObject::invokeMethod(m_controller, "createGui", Qt::BlockingQueuedConnection,
                              Q_ARG(FigureBase*, fig),
                              Q_ARG(Task, *task));
}

/*!
 * \brief ControllerWorker::setProperty
 * \param task
 * \param inGui - Whether to look for the property in the Figure or its GUI
 */
void ControllerWorker::setProperty(Task *task, bool inGui)
{
    if (task->args.length() < 1)
        throw BadArgumentCount(*task, 1, 0);

    QMetaProperty prop = m_controller->findProperty(task, inGui);
    if (!prop.isValid())
        throw InvalidTarget(*task);

    QObject *obj = task->targetObject->object();
    if (!obj)
        throw InvalidNutmegObject(*task, "Property is not attached to a valid object (this is likely a bug in Nutmeg).");

    QVariant value = task->args[0];
    // TODO: Delete commented lines
//    NDArray arr = qvariant_cast<NDArray>(value);
//    qDebug() << "QVariant:" << arr.shape();
    bool succ = prop.write(obj, value);
    if (!succ) {
        // Try construction task type from arg
//        QMetaType propType(prop.type());
//        QMetaType valueType(value.type());
        qDebug() << "Writing property failed:" << prop.typeName() << prop.name();
        qDebug() << "Value:" << value << value.typeName();

        QString msg = QString("Unable to write to property, %1, check values are good.").arg(task->target);
        throw PropertyError(*task, msg, prop.typeName(), prop.name());
    }
}

void ControllerWorker::invoke(Task *task)
{
    QMetaMethod func = m_controller->findMethod(task);

    int nargs = task->args.length();
    if (nargs != func.parameterCount())
        throw InvokeError(*task, QString("Error invoking method %1. Expected %2 args, got %3")
                          .arg(func.methodSignature().constData())
                          .arg(func.parameterCount()).arg(nargs));

    if (nargs > 10)
        throw InvokeError(*task, QString("Error invoking method %1. Maximum of 10 args allowed.")
                          .arg(func.methodSignature().constData()));

    QObject *obj = task->targetObject->object();
    if (!obj)
        throw InvalidNutmegObject(*task, "Method is not attached to a valid object (this is likely a bug in Nutmeg).");

    // Reference: https://gist.github.com/andref/2838534
    QGenericArgument args[10];
    int i;
    for (i=0; i<nargs; ++i) {
        QVariant arg = task->args[i];
        // A const_cast is needed because calling data() would detach the QVariant.
        args[i] = QGenericArgument(arg.typeName(), const_cast<void*>(arg.constData()));
    }

    bool ok = func.invoke(
        obj,
        Qt::DirectConnection,
        args[0],
        args[1],
        args[2],
        args[3],
        args[4],
        args[5],
        args[6],
        args[7],
        args[8],
        args[9]
    );

    if (!ok) {
        QStringList types;
        foreach (auto arg, task->args)
            types << arg.typeName();
        QString typeprint = QString("[%1]").arg( types.join(", ") );
        throw InvokeError(*task,
                          QString("Call to %1 failed (with arg types %2).")
                          .arg(QString(func.methodSignature()))
                          .arg(typeprint));
    }
}

Controller::Controller(QQuickItem *parent)
    : QQuickItem(parent)
    , m_destroying(false)
    , m_components(QMap<QString, QQmlComponent*>())
    , m_figures(QMultiMap<QString,FigureBase*>())
    , m_figuresVar(QVariantMap())
    , m_worker(0)
    , m_taskqueue(new AsyncQueue<Task>())
    , m_figureContainer(0)
    , m_guiContainer(0)
{
    m_worker = new ControllerWorker(this, m_taskqueue);
    connect(m_worker, &ControllerWorker::finished, [=]() { qWarning() << "WARNING! Controller's worker stopped."; });
//    connect(&m_worker, &ControllerWorker::createFigure, this, )
    m_worker->start();
}

Controller::~Controller()
{
    delete m_taskqueue;
    m_destroying = true;
    m_worker->deleteLater();
}

FigureBase *Controller::figure(const QString &handle)
{
    return m_figures.value(handle, 0);
}

QVariantMap Controller::figures() const
{
    return m_figuresVar;
}

/*!
 * \brief
 * Finds the object belonging to the defined target. It is assumed that the last element of target
 * refers to a property or method of the object of interest. Take the following chain as an
 * example: "figure.ax.plt.x". The parts list will look like: ["figure", "ax", "plt", "x"].
 * Controller::findObject will return a pointer to the plt object.
 *
 * \param parts - List of parts of the name chain to the target.
 * \throws InvalidNutmegObject
 * \return The object belonging to the defined target
 */
NutmegObject *Controller::findObject(const QStringList &parts, Task *task, bool inGui)
{
    int N = parts.length() - 1;

    if (N == 0)
        return 0;

    // First find the root object
    FigureBase *fig = m_figures.value(parts[0], 0);
    if (!fig) {
        QString name = "Figure_" + parts[0];
        if (m_components.contains(name)) {
            Task t = *task;
            t.target = parts[0];
            QMetaObject::invokeMethod(this, "createFigure", Qt::BlockingQueuedConnection,
                                      Q_ARG(Task, t),
                                      Q_ARG(QQmlComponent*, m_components[name]));
            fig = m_figures.value(parts[0], 0);
        }
        if (!fig)
            throw FigureNotFoundError(*task, QString("Figure not found with handle, \"%1\"").arg(parts[0]), parts[0]);
    }

    NutmegObject *obj = fig;
    if (inGui) {
        obj = fig->gui();
        if (!obj)
            throw InvalidNutmegObject(*task, QString("GUI not found for figure, \"%1\"").arg(parts[0]));
    }

    // Keep going til the 2nd-last part or we come across a non NutmegObject
    int i;
    for (i=1; i < N; ++i) {
        NutmegObject *child = obj->nutmegChild(parts[i]);
        if (child) {
            obj = child;
            continue;
        }

        // It may also be a property containing a NutmegObject...
        QMetaProperty prop = obj->property(parts[i]);
        QObject *o = obj->object();
        if (!o) {
            QString tar = parts.mid(0, i+1).join('.');
            throw InvalidNutmegObject(*task, QString("No QObject attached to NutmegObject, %1").arg(tar));
        }

        QVariant value = prop.read(o);
        o = qvariant_cast<QObject*>(value);
        obj = dynamic_cast<NutmegObject*>(o);
//        obj = qobject_cast<NutmegObject*>(o);

        if (!obj) {
            qDebug() << "Failed reading:" << prop.typeName() << prop.name();
            qDebug() << value;
            qDebug() << value.canConvert<NutmegObject*>();
            QString tar = parts.mid(0, i+1).join('.');
            throw InvalidNutmegObject(*task, QString("No NutmegObject attached to %1").arg(tar));
        }
    }

    return obj;
}

/*!
 * \brief Controller::findProperty
 * Finds the property pointed to by the task's Task#target member. This will also set the task's
 * Task#targetObject and Task#targetName members in place so that they can be used for completing
 * the task.
 * \param task - Container for information about the task. This method modifies its members.
 * \return The meta property object targetted by the task.
 */
QMetaProperty Controller::findProperty(Task *task, bool inGui)
{
    QStringList parts = task->target.split('.');
    task->targetObject = findObject(parts, task, inGui);

    if (!task->targetObject)
        return QMetaProperty();

    // Return the property
    task->targetName = parts.last();
    QMetaProperty prop = task->targetObject->property(task->targetName);
    if (!prop.isValid()) {
        throw InvalidTarget(*task);
    }

    return prop;
}

/*!
 * \brief Controller::findMethod
 * Finds the method pointed to by the task's Task#target member. This will also set the task's
 * Task#targetObject and Task#targetName members in place so that they can be used for completing
 * the task.
 * \param task - Container for information about the task. This method modifies its members.
 * \return The meta method object targetted by the task.
 */
QMetaMethod Controller::findMethod(Task *task, bool inGui)
{
    QStringList parts = task->target.split('.');
    task->targetObject = findObject(parts, task, inGui);

    if (!task->targetObject)
        return QMetaMethod();

    // Return the property
    task->targetName = parts.last();
    return task->targetObject->method(task->targetName);
}

QQmlComponent *Controller::createQmlComponent(const QByteArray &qml, const QString &name, QQuickItem *parent, const Task &task)
{
    auto ctx = QQmlEngine::contextForObject(parent);
    auto engine = ctx->engine();

    QQmlComponent *comp = new QQmlComponent(engine, parent);
    qDebug() << "Creating component:" << name;
    comp->setData(qml, name);

    int max_t = 200;
    while (comp->isLoading() && --max_t > 0) {
        ServerUtil::sleep(5);
    }

    if (comp->isError()) {
        int line = -1, column = -1;
        QStringList msglist;

        auto errors = comp->errors();
        auto lines = qml.split('\n');

        for (int i=0; i < errors.length(); ++i) {
            // Just grab the first error for now...
            auto error = errors[i];
            line = error.line();
            column = error.column();

            // Split the qml into lines, print the appropriate line, and a helpful arrow to point to it
            if (line > 0 && column > 0) {
                QString arrow = QString("%1^").arg( QString(column - 1, ' ') );

                QString linestr;
                if (i > 0) {
                    linestr = ServerUtil::getLineFromFile(error.url(), line);
                } else {
                    linestr = lines[line - 1];
                }
                msglist << QString("In %1").arg(error.url().toString());
                msglist << error.description() << linestr << arrow;
            } else {
                msglist << error.description();
            }

        }
        QString msg;
        if (msglist.length() > 0) {
            msglist.prepend("");
            msg = msglist.join('\n');
            line = errors[0].line();
            column = errors[0].column();
        } else {
            msg = "Unable to parse QML. No helpful error message was provided...";
        }

        comp->deleteLater();
        throw QmlParserError(task, msg, line, column);

    } else if (!comp->isReady()) {
        comp->deleteLater();
        throw QmlParserError(task, "Unable to parse QML. No helpful error message was provided...", -1, -1);
    }

    // Keep the component around
    if (m_components.contains(name))
        m_components.take(name)->deleteLater();

    m_components[name] = comp;
    return comp;
}

QQuickItem *Controller::createQmlObject(const QByteArray &qml, const QString &name, QQuickItem *parent, const Task &task) {
    auto comp = createQmlComponent(qml, name, parent, task);
    qDebug() << "Component created";
    return createQmlObject(comp, parent, task);
}

QQuickItem *Controller::createQmlObject(QQmlComponent *comp, QQuickItem *parent, const Task &task)
{
    // Begin create and set parent before complete create
    auto ctx = QQmlEngine::contextForObject(parent);
    QObject *obj = comp->beginCreate(ctx);
    QQuickItem *item = qobject_cast<QQuickItem*>(obj);
    if (!item) {
        if (obj)
            obj->deleteLater();
        throw QmlParserError(task, "Unable to create component (likely a bug in Nutmeg).", -1, -1);
    }
    item->setParentItem(parent);
    comp->completeCreate();

    return item;
}

QQuickItem *Controller::figureContainer() const
{
    return m_figureContainer;
}

void Controller::setFigureContainer(QQuickItem *figureContainer)
{
    if (m_figureContainer == figureContainer)
        return;

    m_figureContainer = figureContainer;
    emit figureContainerChanged(figureContainer);
}

QQuickItem *Controller::guiContainer() const
{
    return m_guiContainer;
}

void Controller::setGuiContainer(QQuickItem *guiContainer)
{
    if (m_guiContainer == guiContainer)
        return;

    m_guiContainer = guiContainer;
    emit guiContainerChanged(guiContainer);
}

void Controller::registerFigure(FigureBase *fig)
{
    QString key = fig->handle();
    if (m_figures.contains(key, fig))
        return; // Already in the list, move along now

    m_figures.insert(key, fig);
    updateFigures();
}

void Controller::deregisterFigure(FigureBase *fig)
{
    if (m_destroying) return; // Stops segfaults on close.

    QString key = fig->handle();
    if (!m_figures.contains(key))
        return;

    emit figureDestroyed(fig);
    m_figures.remove(key, fig);
    fig->deleteLater();

    updateFigures();
}

void Controller::queueTask(const QString &command, const QString &target, const QVariantList &args, const QUuid &session, uint64_t id)
{
    Task task(command, target, args, session, id);
    queueTask(task);
}

void Controller::queueTask(const QString &command, const QString &target, const QVariantList &args, int id)
{
    Task task(command, target, args, QUuid::createUuid(), uint64_t(id));
    queueTask(task);
}

void Controller::queueTask(const Task &task)
{
    m_taskqueue->enqueue(task);
}

void Controller::createComponent(Task task)
{
    QByteArray qml = task.args[0].toByteArray();
    try {
        qDebug() << Q_FUNC_INFO;
        QString url = QString("custom/%1.qml").arg(task.target);
        createQmlComponent(qml, url, figureContainer(), task);
        emit successProcessing(task);
        qDebug() << "Done";
    }
    catch (NutmegError &err) {
        emit errorProcessing(err);
    }
}

void Controller::createFigure(Task task, QQmlComponent *comp)
{
    QByteArray qml = task.args[0].toByteArray();
    try {
        qDebug() << "Creating figure";
        QString figname = "Figure_" + task.target;

        bool tryGui = (comp != 0);
        if (!comp) {
            comp = createQmlComponent(qml, figname, figureContainer(), task);
        }
//        QQuickItem *item = createQmlObject(qml, "Figure_" + task.target, figureContainer(), task);
        QQuickItem *item = createQmlObject(comp, figureContainer(), task);
        qDebug() << "Done";
        FigureBase *fig = qobject_cast<FigureBase*>(item);
        if (!fig) {
            if (item)
                item->deleteLater();
            throw FigureError(task, "Root item for figure must be Figure{}.");
        }

        // Set up variables
        fig->setHandle(task.target);
        fig->setController(this);

        registerFigure(fig);

        if (tryGui) {
            QString guiname = "Gui_" + task.target;
            if (m_components.contains(guiname)) {
                createGui(fig, task, m_components[guiname]);
            }
        }

        emit figureCreated(fig);
        emit successProcessing(task);
    }
    catch (NutmegError &err) {
        emit errorProcessing(err);
    }
}

void Controller::createGui(FigureBase *fig, Task task, QQmlComponent *comp)
{
    QByteArray qml = task.args[0].toByteArray();
    try {
        if (!comp) {
            comp = createQmlComponent(qml, "Gui_" + task.target, figureContainer(), task);
        }
        QQuickItem *item = createQmlObject(comp, guiContainer(), task);
//        QQuickItem *item = createQmlObject(qml, "Gui_" + task.target, guiContainer(), task);
        GuiBase *gui = qobject_cast<GuiBase*>(item);

        if (!gui) {
            // Valid qml, but cannot be cast to a GUI Object
            item->deleteLater();
            throw GuiError(task, "Root item for GUI QML must be GUI{}.");
        }

        QString handle = fig->handle();
        fig->setGui(gui);
        gui->setFigureHandle(handle);
        connect(fig, &FigureBase::figureVisibleChanged, gui, &QQuickItem::setVisible );
//        connect(fig, &QQuickItem::visibleChanged, [=](){ gui->setVisible(fig->isVisible()); } );
        connect(gui, &GuiBase::parameterChanged, this, &Controller::parameterUpdated);

        foreach (QString param, gui->parameterList()) {
            parameterUpdated(handle, param, gui->parameter(param)->value());
        }

        emit successProcessing(task);
    }
    catch (NutmegError &err) {
        emit errorProcessing(err);
    }
}

//void Controller::createFigure(const Task &task)
//{

//}

void Controller::updateFigures()
{
    QVariantMap newMap; // Map of QVariantList
    foreach (const QString &key, m_figures.keys()) {
        QVariantList list;
        foreach (QObject *fig, m_figures.values(key))
            list.prepend(QVariant::fromValue(fig));
        newMap.insert(key, list);
    }
    m_figuresVar = newMap;
    emit figuresChanged(m_figuresVar);
}

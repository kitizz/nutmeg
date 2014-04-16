#include "figurebase.h"

#include <QVariant>

FigureBase::FigureBase(QQuickItem *parent)
    : QQuickItem(parent)
    , m_destroying(false)
    , m_handle(QString())
    , m_controller(0)
{
    m_axes = QMultiMap<QString,AxisBase*>();
    m_axesVar = QVariantMap();
    m_axisList = QVariantList();

    QMap<QString,QString> props;
    registerProperties(props);
}

FigureBase::~FigureBase()
{
    if (m_controller)
        m_controller->deregisterFigure(this);
    m_destroying = true;
}

//void FigureBase::paint(QPainter *painter)
//{

//}

QVariantMap FigureBase::axes() const
{
    return m_axesVar;
}

/**
 * @brief FigureBase::getAxesByHandle
 * @param handle The handle of the axes you wish to access.
 * @return A list of the axes with the requested handle if handle is not empty,
 *      else all the axes belonging to the figure.
 */
QList<AxisBase*> FigureBase::getAxesByHandle(const QString &handle) const
{
    // TODO: This is O(n) complexity at the moment...
    QList<AxisBase*> values;
    if (handle.isEmpty())
        values = m_axes.values();
    else
        values = m_axes.values(handle);

    QList<AxisBase*> result;
    foreach (AxisBase* ax, values)
        result.prepend(ax);

    return result;
}

void FigureBase::registerAxis(AxisBase *axis)
{
    QString key = axis->handle();
    qDebug() << "FigureBase::registerAxis" << key << axis;
    if (m_axes.contains(key, axis))
        return; // Already in the list, move along now

    m_axes.insert(key, axis);
    updateAxes();
}

QVariantList FigureBase::getAxisList()
{
    return m_axisList;
}

void FigureBase::deregisterAxis(AxisBase *axis)
{
    if (m_destroying) return; // Stops segfaults on close.
    QString key = axis->handle();
    m_axes.remove(key, axis);
    updateAxes();
}

QString FigureBase::handle() const
{
    return m_handle;
}

void FigureBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

Controller *FigureBase::controller() const
{
    return m_controller;
}

void FigureBase::setController(Controller *arg)
{
    qDebug() << "setController";
    if (m_controller == arg) return;

    if (m_controller)
        m_controller->deregisterFigure(this);

    m_controller = arg;

    if (m_controller)
        m_controller->registerFigure(this);

    emit controllerChanged(arg);
}

QString FigureBase::map(QString prop)
{
    return NutmegObject::map(prop);
}

void FigureBase::installEventFilterApp(QObject *app)
{
    if (!app) return;
    app->installEventFilter(this);
}

bool FigureBase::eventFilter(QObject *watched, QEvent *event)
{
//    if (event->type() == QEvent::MouseMove) {
//        // Record the mouse info
//        MouseEvent *mouseEvent = new MouseEvent(event);
//        m_mouse = mapFromScene(QPointF(mouseEvent->x(), mouseEvent->y()));
//        emit mouseChanged(m_mouse);
//        emit mouseMoved(mouseEvent);

//    } else if (event->type() == QEvent::MouseButtonPress) {
//        MouseEvent *mouseEvent = new MouseEvent(event);
//        // Dragging is for the left-button only
//        if (mouseEvent->button() == Qt::LeftButton) {
//            setMouseStart(m_mouse); // Plain and simple..
//            setMouseDelta(QPointF());
//        }

//        m_mouseButtons = mouseEvent->buttons();
//        emit mouseButtonsChanged(m_mouseButtons);
//        emit mousePressed(mouseEvent);

//    } else if (event->type() == QEvent::MouseButtonRelease) {
//        MouseEvent *mouseEvent = new MouseEvent(event);
//        m_mouseButtons = mouseEvent->buttons();
//        emit mouseButtonsChanged(m_mouseButtons);
//        emit mouseReleased(mouseEvent);

//        // Clicking is for the left-button only
//        if (mouseEvent->button() == Qt::LeftButton) {
//            QPointF frameDelta = m_mouseFrame - spaceToFrame(m_mouseStart);
//            if (frameDelta.manhattanLength() < 3)
//                emit mouseClicked(mouseEvent);
//        }
//    }
    return QQuickItem::eventFilter(watched, event);
}

QPointF FigureBase::mouse() const
{
    return m_mouse;
}

void FigureBase::setMouse(QPointF arg)
{
    if (m_mouse == arg) return;
    m_mouse = arg;
    emit mouseChanged(arg);
}

QPointF FigureBase::mouseUnit() const
{
    return m_mouseUnit;
}

void FigureBase::setMouseUnit(QPointF arg)
{
    if (m_mouseUnit == arg) return;
    m_mouseUnit = arg;
    emit mouseUnitChanged(arg);
}

int FigureBase::mouseButtons() const
{
    return m_mouseButtons;
}

void FigureBase::setMouseButtons(int arg)
{
    if (m_mouseButtons == arg) return;
    m_mouseButtons = arg;
    emit mouseButtonsChanged(arg);
}

void FigureBase::updateAxes()
{
    QVariantMap newMap; // Map of QVariantList
    QVariantList newList;
    foreach (const QString &key, m_axes.keys()) {
        QVariantList list;
        foreach (QObject *ax, m_axes.values(key)) {
            list.prepend(QVariant::fromValue(ax));
            newList.prepend(QVariant::fromValue(ax));
        }
        newMap.insert(key, list);
    }
    m_axesVar = newMap;
    m_axisList = newList;
    emit axesChanged(m_axesVar);
}

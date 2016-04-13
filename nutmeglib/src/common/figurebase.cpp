#include "figurebase.h"

#include <QVariant>
#include <QPrinter>
#include <QPainter>

#include "util.h"
#include <src/server/controller.h>
#include <src/common/axisbase.h>
#include <src/2d/axes/axis2dbase.h>

FigureBase::FigureBase(QQuickItem *parent)
    : QQuickItem(parent)
    , NutmegObject(this)
    , m_axesVar(QVariantMap())
    , m_axisList(QVariantList())
    , m_axes(QMultiMap<QString,AxisBase*>())
    , m_destroying(false)
    , m_handle(QString())
    , m_controller(0)
    , m_mouse(QPointF())
    , m_mouseUnit(QPointF())
    , m_mouseButtons(0)
    , m_qml(QByteArray())
    , m_gui(0)
{
    registerChildMethod("axis");
}

FigureBase::~FigureBase()
{
    if (m_controller)
        m_controller->deregisterFigure(this);
    foreach (AxisBase *ax, m_axes.values()) {
        deregisterAxis(ax);
        delete ax;
    }

    m_destroying = true;
}

AxisBase *FigureBase::axis(const QString &handle)
{
    return m_axes.value(handle, 0);
}

/*!
 * \property FigureBase::axes
 * A map of Axis handles to the actual Axes.
 */
QVariantMap FigureBase::axes() const
{
    return m_axesVar;
}

/*! \internal
 * \fn FigureBase::getAxesByHandle
 * \param handle The handle of the axes you wish to access.
 * \return A list of the axes with the requested handle if handle is not empty,
 *      else all the axes belonging to the figure.
 */
QList<AxisBase *> FigureBase::getAxesByHandle(const QString &handle) const
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
        return; // This key-axis combo are already here, move along now

    m_axes.insert(key, axis);
    updateAxes();
}

/*! \internal
 * \fn FigureBase::registerAxisGroup
 * Register a 1D or 2D array of axes as a group. Must define the type of group
 * (column, row, grid).
 * \return The name of group generated.
 */
// TODO: Remove?
//QString FigureBase::registerAxisGroup(QVariantList axisGroup, AxisGroupType type, const QString &oldGroupName)
//{
//    AxisGroup *newGroup = new AxisGroup(axisGroup, type);
//    // TODO: This is risk of name collisions here. Unlikely but possible

//    if (newGroup->isValid()) {
//        if (m_axisGroups.contains(oldGroupName))
//            delete m_axisGroups.take(oldGroupName);

//        m_axisGroups.insert(newGroup->name, newGroup);
//        return newGroup->name;

//    } else {
//        if (m_axisGroups.contains(oldGroupName)) {
//            AxisGroup *oldGroup = m_axisGroups.take(oldGroupName);
//            oldGroup->unbindAxes();
//            delete oldGroup;
//        }
//        delete newGroup;
//        return "";
//    }

//}

//void FigureBase::updateGroupAt(const QString &name, int row, int column)
//{
//    AxisGroup *group = m_axisGroups[name];
//    if (!group) return;
//    int rows, columns;
//    group->getSize(rows, columns);

//    // Column first
//    for (int i=0; i<rows; ++i) {
//        if (i == row) continue; // Skip self
//        group->axes[i][column]->triggerRedraw();
//    }
//    // Then row
//    for (int j=0; j<columns; ++j) {
//        if (j == column) continue; // Skip self
//        group->axes[row][j]->triggerRedraw();
//    }
//}

void FigureBase::updateShareX(Axis2DBase *axis)
{
    QString tag = axis->shareX();

    qreal minX = axis->minX(), maxX = axis->maxX();
    foreach (AxisBase *axB, m_axes.values()) {
        Axis2DBase *ax = qobject_cast<Axis2DBase*>(axB);
        if (!ax || ax == axis || ax->shareX() != tag)
            continue; // Skip the calling Axis and non-shared axes
        // We set the new limits like this so that only 1 update is made
        QRectF lim = ax->limits();
        lim.setLeft(minX);
        lim.setRight(maxX);
        ax->setLimits(lim, false, true);
    }
}

void FigureBase::updateShareY(Axis2DBase *axis)
{
    QString tag = axis->shareX();

    qreal minY = axis->minY(), maxY = axis->maxY();
    foreach (AxisBase *axB, m_axes.values()) {
        Axis2DBase *ax = qobject_cast<Axis2DBase*>(axB);
        if (!ax || ax == axis || ax->shareY() != tag)
            continue; // Skip the calling Axis and non-shared axes
        // We set the new limits like this so that only 1 update is made
        QRectF lim = ax->limits();
        lim.setTop(minY);
        lim.setBottom(maxY);
        ax->setLimits(lim, false, true);
    }
}

void FigureBase::savePdf(QString filepath)
{
    qDebug() << "Writing pdf:" << filepath;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filepath);

    QPainter p;
    p.begin(&printer);

    QPen pen;
    pen.setWidth(10);
    pen.setColor(QColor(0,0,0));
    p.setPen(pen);
    p.drawLine(0,0, 100,500);

    foreach (AxisBase *axis, m_axes) {
        qDebug() << "Paint axis" << axis;
        axis->print(&p);
    }
    p.end();
}

QVariantList FigureBase::getAxisList()
{
    return m_axisList;
}

/*! \internal
 * If this Axis belongs to a group, this returns a QRectF whose height is
 * bounded by the smallest height in its row, and width is bounded by the
 * smallest width in its coloumn.
 */
//QRectF FigureBase::getPlotRectFor(AxisBase *axis) const
//{
//    AxisGroup *group = axis->layoutGroup();
//    QRectF axisRect = axis->preferredPlotRect();
//    if (!group)
//        return axisRect;

//    int rows, cols;
//    group->getSize(rows, cols);

//    int row, col;
//    axis->getAxisGroupIndex(row, col);

//    // In these loops, we could check for i==col, j==row, and skip.
//    // However, it's good to reduce branching and this is
//    // unlikely to have a significant impact on performance.

//    // First check the column
//    qreal minX = axisRect.left(), maxX = axisRect.right();
//    for (int i=0; i<rows; ++i) {
//        AxisBase *other = group->axes[i][col];
//        QRectF r = other->preferredPlotRect();
//        minX = qMax(minX, r.left());
//        maxX = qMin(maxX, r.right());
//    }

//    // Now check the row
//    qreal minY = axisRect.top(), maxY = axisRect.bottom();
//    for (int j=0; j<cols; ++j) {
//        AxisBase *other = group->axes[row][j];
//        QRectF r = other->preferredPlotRect();
//        minY = qMax(minY, r.top());
//        maxY = qMin(maxY, r.bottom());
//    }

//    return QRectF(minX, minY, maxX - minX, maxY - minY);
//}

void FigureBase::deregisterAxis(AxisBase *axis)
{
    if (m_destroying) return; // Stops segfaults on close.
    QString key = axis->handle();
    m_axes.remove(key, axis);
    updateAxes();
}

/*!
 * \property FigureBase::handle
 * The handle of the figure to be referenced from the client code.
 */
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

void FigureBase::installEventFilterApp(QObject *app)
{
    qDebug() << "Installed event filter into app:" << app;
    if (!app) return;
    app->installEventFilter(this);
}

bool FigureBase::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::TouchBegin ||
            event->type() == QEvent::TouchEnd ||
            event->type() == QEvent::TouchUpdate) {
//        qDebug() << "Event occured!" << event << "\n";
        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent*>(event)->touchPoints();
    }
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

QByteArray FigureBase::qml() const
{
    return m_qml;
}

void FigureBase::setQml(QByteArray qml)
{
    if (m_qml == qml)
        return;

    m_qml = qml;
    emit qmlChanged(qml);
}

GuiBase *FigureBase::gui() const
{
    return m_gui;
}

void FigureBase::setGui(GuiBase *gui)
{
    if (m_gui == gui)
        return;

    m_gui = gui;
    emit guiChanged(gui);
}

NutmegObject *FigureBase::nutmegChild(const QString &name)
{
    return axis(name);
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


//AxisGroup::AxisGroup(QVariantList axisGroup, FigureBase::AxisGroupType type)
//    : name(QString())
//    , axes(QVector< QVector<AxisBase*> >())
//    , m_rows(0)
//    , m_cols(0)
//{
//    // Whether this is a row, coloumn or grid group, the input must be 2D
//    int nRows = type == FigureBase::Row ? 1 : axisGroup.length();
//    int nCols = 1;  // 1 for type == Column
//    if (type == FigureBase::Row)
//        nCols = axisGroup.length();

//    QStringList axisNames;

//    // This could be condensed into one for loop with embedded if-statements
//    // but this makes it much easier to read the separate cases.
//    // Also, it's probably faster to have the ifs on the outside.
//    if (type == FigureBase::Column) {
//        for (int i=0; i<nRows; ++i) {
//            QVector<AxisBase*> row;
//            AxisBase *ax = qvariant_cast<AxisBase*>(axisGroup[i]);
//            if (!ax)
//                qWarning() << "Warning: Cannot register Axis Column. Found non-AxisBase entry.";
//            axisNames.append(ax->handle());
//            ax->setLayoutGroup(this);
//            ax->setAxisGroupIndex(i, 0);
//            row.append(ax);
//            axes.append(row);
//        }

//    } else if (type == FigureBase::Grid) {
//        // Double loop through the axisGroup, making the right conversions
//        for (int i=0; i<nRows; ++i) {
//            QVector<AxisBase*> row;

//            QVariantList row_var = axisGroup[i].toList();
//            int len = row_var.length();
//            if (i == 0)
//                nCols = len;
//            if (len == 0 || len != nCols) {
//                qWarning() << "Warning: Cannot register Axis Grid. Each row in grid"
//                           << "must be of the same size.";
//                return;
//            }
//            for (int j=0; j<len; ++j) {
//                AxisBase *ax = qvariant_cast<AxisBase*>(row_var[j]);
//                if (!ax)
//                    qWarning() << "Warning: Cannot register Axis Grid. Found non-AxisBase entry.";
//                axisNames.append(ax->handle());
//                ax->setLayoutGroup(this);
//                ax->setAxisGroupIndex(i, j);
//                row.append(ax);
//            }
//            axes.append(row);
//        }

//    } else if (type == FigureBase::Row) {
//        QVector<AxisBase*> row;
//        for (int i=0; i<nRows; ++i) {
//            AxisBase *ax = qvariant_cast<AxisBase*>(axisGroup[i]);
//            if (!ax)
//                qWarning() << "Warning: Cannot register Axis Row. Found non-AxisBase entry.";
//            axisNames.append(ax->handle());
//            ax->setLayoutGroup(this);
//            ax->setAxisGroupIndex(0, i);
//            row.append(ax);
//        }
//        axes.append(row);
//    }

//    name = QStringList(axisNames.toSet().toList()).join('-');

//    m_rows = nRows;
//    m_cols = nCols;
//}

//void AxisGroup::unbindAxes()
//{
//    foreach (QVector<AxisBase*> lst, axes) {
//        foreach (AxisBase* ax, lst) {
//            ax->setLayoutGroup(0);
//            ax->setAxisGroupIndex(-1,-1);
//        }
//    }
//}

//void AxisGroup::getSize(int &rows, int &columns)
//{
//    rows = m_rows;
//    columns = m_cols;
//}

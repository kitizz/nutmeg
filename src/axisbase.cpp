#include "axisbase.h"

AxisBase::AxisBase(QQuickItem *parent)
    : AxisBase(parent)
{

}

/*!
 * \property AxisBase::handle
 * The handle property provides Nutmeg a way of identifying this Axis.
 * By default, this is "axis".
 */
QString AxisBase::handle() const
{
    return m_handle;
}

void AxisBase::setHandle(QString arg)
{
    if (m_handle == arg) return;
    m_handle = arg;
    emit handleChanged(arg);
}

void AxisBase::setFigure(FigureBase *arg)
{
    if (m_figure == arg) return;

    if (m_figure) {
        m_figure->deregisterAxis(this);
    }

    m_figure = arg;

    if (m_figure) {
        m_figure->registerAxis(this);
    }
//    else {
//        qWarning() << Q_FUNC_INFO << "Axis is not a descendant of any Figure. It may not behave as expected.";
//    }

    emit figureChanged(arg);
}

void AxisBase::updateFigure()
{
    // Disconnect from an external sender
    QObject *sender = QObject::sender();
    if (sender && this != sender)
        sender->disconnect(this);

    // Work up the tree until the next Figure item is found.
    QQuickItem *newParent = parentItem();
    if (!newParent) return; // Either being deleted or instantiated
    FigureBase *figure;
    while (true) {
        figure = qobject_cast<FigureBase*>(newParent);
        if (figure) break;
        if (!newParent->parentItem()) {
            connect(newParent, &QQuickItem::parentChanged, this, &AxisBase::updateFigure);
            break;
        }
        newParent = newParent->parentItem();
    }

    setFigure(figure);
}


/*!
 * \property AxisBase::title
 * Set the title of this axis.
 */
QString AxisBase::title() const
{
    return m_title;
}

void AxisBase::setTitle(QString arg)
{
    if (m_title == arg) return;
    m_title = arg;
    emit titleChanged(arg);
}

/*!
 * \property AxisBase::titleFont
 * Define the Font to be used for the title.
 * Can be used as a group property:
 * \code{.qml}
 * titleFont { bold: true, family: "Arial", pointSize: 16 }
 * \endcode
 * See QML's documentation for details on its basic Font type:
 * http://qt-project.org/doc/qt-5/qml-font.html
 */
Font AxisBase::titleFont() const
{
    return m_titleFont;
}

void AxisBase::setTitleFont(Font arg)
{
    if (m_titleFont == arg) return;
    m_titleFont = arg;
    emit titleFontChanged(arg);
}

/*!
 * \property AxisBase::titleColor
 * The color of the title text.
 */
QColor AxisBase::titleColor() const
{
    return m_titleColor;
}

void AxisBase::setTitleColor(QColor arg)
{
    if (m_titleColor == arg) return;
    m_titleColor = arg;
    emit titleColorChanged(arg);
}

FigureBase *AxisBase::figure() const
{
    return m_figure;
}

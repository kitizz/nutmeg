#ifndef FIGURE_H
#define FIGURE_H

#include <QQuickItem>
#include <QMultiMap>
#include <QFileDialog>

#include "nutmegobject.h"
#include "axisbase.h"
#include "guibase.h"
#include "../util/mouseevent.h"

class AxisBase;
class Axis2DBase;
class Controller;
class LayoutGrid;
class GuiItem;
class Controller;
class NUTMEGLIB_EXPORT FigureBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QVariantMap axes READ axes NOTIFY axesChanged)
    Q_PROPERTY(Controller* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(int mouseButtons READ mouseButtons NOTIFY mouseButtonsChanged)
    Q_PROPERTY(QPointF mouse READ mouse NOTIFY mouseChanged)
    Q_PROPERTY(QPointF mouseUnit READ mouseUnit NOTIFY mouseUnitChanged)
    Q_PROPERTY(QByteArray qml READ qml WRITE setQml NOTIFY qmlChanged)
    Q_PROPERTY(GuiBase* gui READ gui WRITE setGui NOTIFY guiChanged)


public:
    explicit FigureBase(QQuickItem *parent = 0);
    ~FigureBase();

    enum AxisGroupType { Row=1, Column=2, Grid=3 };
    Q_ENUM(AxisGroupType);

//    void paint(QPainter *painter);

    AxisBase *axis(const QString &handle);
    QVariantMap axes() const; // Use this for QML
    QList<AxisBase*> getAxesByHandle(const QString &handle) const; // Use this for c++
    Q_INVOKABLE QVariantList getAxisList();
//    QRectF getPlotRectFor(AxisBase* axis) const;

    QString handle() const;
    void setHandle(QString arg);

    Controller* controller() const;
    void setController(Controller* arg);

    Q_INVOKABLE void installEventFilterApp(QObject *app);
    bool eventFilter(QObject *watched, QEvent *event);

    QPointF mouse() const;
    void setMouse(QPointF arg);

    QPointF mouseUnit() const;
    void setMouseUnit(QPointF arg);

    int mouseButtons() const;
    void setMouseButtons(int arg);

    QByteArray qml() const;
    void setQml(QByteArray qml);

    GuiBase *gui() const;
    void setGui(GuiBase *gui);

    virtual NutmegObject *nutmegChild(const QString &name);

signals:
    void figureVisibleChanged(bool vis);
    void mouseMoved(MouseEvent* mouse);
    void mousePressed(MouseEvent* mouse);
    void mouseReleased(MouseEvent* mouse);
    void axesChanged(QVariantMap arg);
    void handleChanged(QString arg);
    void addedAxis(AxisBase* axis);
    void controllerChanged(Controller* arg);
    void mouseChanged(QPointF arg);
    void mouseUnitChanged(QPointF arg);
    void mouseButtonsChanged(int arg);

    void qmlChanged(QString qml);
    void guiChanged(QQuickItem* gui);

public slots:
    void deregisterAxis(AxisBase *axis);
    void registerAxis(AxisBase *axis);

//    QString registerAxisGroup(QVariantList axisGroup, AxisGroupType type, const QString &oldGroup="");
//    void updateGroupAt(const QString &name, int row, int column);

    void updateShareX(Axis2DBase *axis);
    void updateShareY(Axis2DBase *axis);

    void savePdf(QString filepath);

// TODO: Remove once Q_GADGET working properly...
//    virtual void registerProperties(QMap<QString, QString> mapping) {
//        NutmegObject::registerProperties(mapping);
//    }
//    virtual void registerProperties(QVariantMap mapping) {
//        NutmegObject::registerProperties(mapping);
//    }
//    virtual QString mapProperty(const QString &prop) {
//        return NutmegObject::mapProperty(prop);
//    }

//    virtual void registerMethods(QMap<QString, QString> mapping) {
//        NutmegObject::registerMethods(mapping);
//    }
//    virtual QString mapMethod(const QString &method) {
//        return NutmegObject::mapMethod(method);
//    }



protected slots:
    void updateAxes();

private:
    QVariantMap m_axesVar;
    QVariantList m_axisList;
    QMultiMap<QString,AxisBase*> m_axes;
//    QMap<QString, AxisGroup*> m_axisGroups;
    bool m_destroying;

    QString m_handle;
    Controller* m_controller;
    QPointF m_mouse;
    QPointF m_mouseUnit;
    int m_mouseButtons;
    QByteArray m_qml;
    GuiBase* m_gui;
};

//class AxisGroup
//{
//public:
//    AxisGroup(QVariantList axisGroup, FigureBase::AxisGroupType type);

//    bool isValid() { return m_rows > 0 && m_cols > 0; }
//    void unbindAxes();

//    QString name;
//    QVector< QVector<AxisBase*> > axes;
//    void getSize(int &rows, int &columns);

//private:
//    int m_rows, m_cols;
//};

#endif // FIGURE_H

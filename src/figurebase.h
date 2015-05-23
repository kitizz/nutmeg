#ifndef FIGURE_H
#define FIGURE_H

#include <QQuickItem>
#include <QMultiMap>
#include <QFileDialog>
#include "axisbase.h"
#include "axis2dbase.h"
#include "controller.h"
#include "nutmegobject.h"
#include "mouseevent.h"

class AxisBase;
class Axis2DBase;
class Controller;
class AxisGroup;
class FigureBase : public QQuickItem, public NutmegObject
{
    Q_OBJECT
    Q_PROPERTY(QString handle READ handle WRITE setHandle NOTIFY handleChanged)
    Q_PROPERTY(QVariantMap axes READ axes NOTIFY axesChanged)
    Q_PROPERTY(Controller* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(int mouseButtons READ mouseButtons NOTIFY mouseButtonsChanged)
    Q_PROPERTY(QPointF mouse READ mouse NOTIFY mouseChanged)
    Q_PROPERTY(QPointF mouseUnit READ mouseUnit NOTIFY mouseUnitChanged)
    Q_ENUMS(AxisGroupType)

public:
    explicit FigureBase(QQuickItem *parent = 0);
    ~FigureBase();

    enum AxisGroupType { Row=1, Column=2, Grid=3 };

//    void paint(QPainter *painter);

    QVariantMap axes() const; // Use this for QML
    QList<AxisBase*> getAxesByHandle(const QString &handle) const; // Use this for c++
    Q_INVOKABLE QVariantList getAxisList();
    QRectF getPlotRectFor(AxisBase* axis) const;

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

signals:
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

public slots:
    void deregisterAxis(AxisBase *axis);
    void registerAxis(AxisBase *axis);

    QString registerAxisGroup(QVariantList axisGroup, AxisGroupType type, const QString &oldGroup="");
    void updateGroupAt(const QString &name, int row, int column);

    void updateShareX(Axis2DBase *axis);

    void savePdf(QString filepath);

    // Nutmeg object. TODO: Remove once Q_GADGET working properly...
    virtual void registerProperties(QMap<QString, QString> mapping) {
        NutmegObject::registerProperties(mapping);
    }
    virtual void registerProperties(QVariantMap mapping) {
        NutmegObject::registerProperties(mapping);
    }
    virtual QString mapProperty(const QString &prop) {
        return NutmegObject::mapProperty(prop);
    }

    virtual void registerMethods(QMap<QString, QString> mapping) {
        NutmegObject::registerMethods(mapping);
    }
    virtual QString mapMethod(const QString &method) {
        return NutmegObject::mapMethod(method);
    }

protected slots:
    void updateAxes();

private:
    QVariantMap m_axesVar;
    QVariantList m_axisList;
    QMultiMap<QString,AxisBase*> m_axes;
    QMap<QString, AxisGroup*> m_axisGroups;
    bool m_destroying;

    QString m_handle;
    Controller* m_controller;
    QPointF m_mouse;
    QPointF m_mouseUnit;
    int m_mouseButtons;
};

class AxisGroup
{
public:
    AxisGroup(QVariantList axisGroup, FigureBase::AxisGroupType type);

    bool isValid() { return m_rows > 0 && m_cols > 0; }
    void unbindAxes();

    QString name;
    QVector< QVector<AxisBase*> > axes;
    void getSize(int &rows, int &columns);

private:
    int m_rows, m_cols;
};

#endif // FIGURE_H

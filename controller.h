#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QQuickItem>

#include "figurebase.h"

class FigureBase;
class Controller : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap figures READ figures NOTIFY figuresChanged)

public:
    explicit Controller(QQuickItem *parent = 0);
    ~Controller();

    QVariantMap figures() const;
    QList<FigureBase*> getFiguresByHandle(const QString &handle);

signals:
    void figuresChanged(QVariantMap arg);

public slots:
    void deregisterFigure(FigureBase *fig);
    void registerFigure(FigureBase *fig);

protected slots:
    void updateFigures();

private:
    bool m_destroying;
    QMultiMap<QString,FigureBase*> m_figures;
    QVariantMap m_figuresVar;

};

#endif // CONTROLLER_H

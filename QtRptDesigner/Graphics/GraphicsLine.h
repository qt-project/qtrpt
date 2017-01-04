#ifndef GRAPHICSLINE_H
#define GRAPHICSLINE_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include "CornerGrabber.h"
#include <QTransform>
#include <QDomDocument>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include "GraphicsNameSpace.h"
#include "GraphicsHelperClass.h"

class GraphicsLine : public QGraphicsPolygonItem, public GraphicsHelperClass
{
public:
    GraphicsLine();
    void setGridSpace(int space);
    qreal getLength();
    void setLength(qreal value);
    void setEndPosition(QPointF endPoint);
    virtual void setPos(QPointF pos);
    virtual void setPos(qreal x, qreal y);
    void setSelected(bool selected);
    bool isSelected();
    void setBorderColor(QColor value) {m_borderColor = value;}
    int type() const Q_DECL_OVERRIDE { return ItemType::GLine; }
    QList<QPointF> getPointList() {return m_pointList;}
    void setPointList(QList<QPointF> list) {m_pointList = list;}
    void addPoint(QPointF point) {m_pointList << point;}
    bool getArrowStart() {return m_arrowStart;}
    void setArrowStart(bool value) {m_arrowStart = value;}
    bool getArrowEnd() {return m_arrowEnd;}
    void setArrowEnd(bool value) {m_arrowEnd = value;}
    void setArrow(QtRptName::Command command, QVariant value);
    bool getArrow(QtRptName::Command command);
    GraphicsLine *clone();
    void loadParamFromXML(QDomElement e);
    QDomElement saveParamToXML(QDomDocument *xmlDoc);
    void setMenu(QMenu *menu_);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
   // virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );///< allows the main object to be moved in the scene by capturing the mouse move events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) ;

    void setCornerPositions();
    void createCustomPath(QPointF mouseLocation, CornerGrabber*);

    QColor  _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen    _pen; ///< the pen is used to paint the red/black border
    QPointF _location;
    QPointF _dragStart;

    int _XcornerGrabBuffer;
    int _YcornerGrabBuffer;

    qreal   _graphicsItemBoundingBoxWidth;
    QList<QPointF> m_pointList;

    bool _cornerGrabbed;
    bool m_arrowStart;
    bool m_arrowEnd;

    CornerGrabber*  _corners[2];
    void createCorners();
    void destroyCorners();
    QPolygonF _selectRegion;
    void initPolygon();

};


#endif // GRAPHICSLINE_H

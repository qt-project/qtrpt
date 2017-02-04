#include "GraphicsLine.h"
#include <QBrush>
#include <QLinearGradient>
#include <QDebug>
#include <QRect>
#include <QPolygon>
#include <QGraphicsPathItem>
#include "GraphicsScene.h"
#include "math.h"

GraphicsLine::GraphicsLine(): QGraphicsPolygonItem(),
        _pen(),
        _location(0,0),
        _dragStart(0,0),
        _XcornerGrabBuffer(20),
        _YcornerGrabBuffer(20),
        _graphicsItemBoundingBoxWidth( 200 +(2 * _XcornerGrabBuffer)),
        _cornerGrabbed(false),
        _selectRegion()
{
    setDrawingState(true);

    m_outterborderColor = Qt::black;
    m_borderColor = Qt::black;

    m_corners.resize(2);
    for (auto& corner : m_corners)
        corner = nullptr;

    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,true);

    _pen.setWidth(2);
    _pen.setColor(m_outterborderColor);

    this->setAcceptHoverEvents(true);

    this->graphicsItem = this;
}

void GraphicsLine::initPolygon() {
    _selectRegion.clear();
    for (int p=0; p<m_pointList.size(); p++) {
        QPointF p1  (m_pointList[p].x() - _XcornerGrabBuffer, m_pointList[p].y() - _YcornerGrabBuffer);
        QPointF p2  (m_pointList[p].x() + _XcornerGrabBuffer, m_pointList[p].y() - _YcornerGrabBuffer);

        QPointF p3  (m_pointList[p].x() + _XcornerGrabBuffer, m_pointList[p].y() + _YcornerGrabBuffer);
        QPointF p4  (m_pointList[p].x() - _XcornerGrabBuffer, m_pointList[p].y() + _YcornerGrabBuffer);

        _selectRegion << p1 << p2 << p3 << p4 << p1 << p2;
    }
    for (int p=m_pointList.size()-1; p>-1; p--) {
        QPointF p3  (m_pointList[p].x() + _XcornerGrabBuffer, m_pointList[p].y() + _YcornerGrabBuffer);
        QPointF p4  (m_pointList[p].x() - _XcornerGrabBuffer, m_pointList[p].y() + _YcornerGrabBuffer);
        _selectRegion << p3 << p4;
    }
    QPointF p1  (m_pointList[0].x() - _XcornerGrabBuffer, m_pointList[0].y() - _YcornerGrabBuffer);
    _selectRegion << p1;

    this->setPolygon(_selectRegion);
    this->update();
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */
bool GraphicsLine::sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) {
    //qDebug() << " QEvent == " + QString::number(event->type());

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL) {
        // this is not one of the mouse events we are interrested in
        return false;
    }

    switch (event->type())  {
        // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress: {
        corner->setMouseState(CornerGrabber::kMouseDown);

        QPointF scenePosition =  corner->mapToScene(mevent->pos());
        corner->mouseDownY = scenePosition.y();
        corner->mouseDownX= scenePosition.x();

        _cornerGrabbed = true;

        //Посылаем сигнал в сцену для отслеживания Ундо при перемещении концов
        GraphicsScene *model = qobject_cast<GraphicsScene *>(scene());
        if (model)
        	model->itemMoving(this);
        break;
	}
    case QEvent::GraphicsSceneMouseRelease: {
        corner->setMouseState(CornerGrabber::kMouseReleased);
        _cornerGrabbed = false;
        break;
	}
    case QEvent::GraphicsSceneMouseMove: {
        corner->setMouseState(CornerGrabber::kMouseMoving );
        break;
	}
    default:
        // we dont care about the rest of the events
        return false;
        break;
    }

    if ( corner->getMouseState() == CornerGrabber::kMouseMoving ) {
        GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
        m_scene->itemResizing(this);

        createCustomPath(mevent->pos(), corner);
        this->scene()->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}

void GraphicsLine::setPos(QPointF pos) {
    _location = pos;
    QGraphicsItem::setPos(pos);
}

void GraphicsLine::setPos(qreal x, qreal y) {
    setPos(QPoint(x,y));
}

void GraphicsLine::setEndPosition(QPointF endPoint) {
    m_pointList[m_pointList.size()-1] = endPoint;
    initPolygon();
    setCornerPositions();
    //createCustomPath(mapToItem(_corners[1], endPoint), _corners[1]) ;
}

void GraphicsLine::createCustomPath(QPointF mouseLocation, CornerGrabber* corner) {
    if (corner == 0) return;
    QPointF scenePosition = corner->mapToScene(mouseLocation);

    // which corner needs to get moved?
    int idx = -1;
    for (int p=0; p<m_pointList.size(); p++) {
        if ( corner == m_corners[p]) {
            idx = p;
            m_pointList[p] = mapFromScene(scenePosition);
        }
    }
    initPolygon();
    if (idx > -1) {
        int cornerWidth = (m_corners[0]->boundingRect().width())/2;
        int cornerHeight = ( m_corners[0]->boundingRect().height())/2;
        m_corners[idx]->setPos(m_pointList[idx].x() - cornerWidth, m_pointList[idx].y() - cornerHeight );
    }
}

void GraphicsLine::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ) {
    this->scene()->update();
}

void GraphicsLine::hoverEnterEvent ( QGraphicsSceneHoverEvent * ) {
    this->scene()->update();
}

// for supporting moving the box across the scene
void GraphicsLine::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(true);
    this->setPos(_location);
    this->setSelected(true);
}

// for supporting moving the box across the scene
void GraphicsLine::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(true);
    _dragStart = event->pos();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setSelected(true);
}

// for supporting moving the box across the scene
void GraphicsLine::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    this->setPos(_location);

    GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
    m_scene->itemResizing(this);
    this->scene()->update();
}

void GraphicsLine::setSelected(bool selected_) {
    QGraphicsItem::setSelected(selected_);
    if (itemInTree != nullptr)
        itemInTree->setSelected(selected_);

    if (selected_) {
        createCorners();
        GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
        emit m_scene->itemSelected(this);
        this->setZValue(3);
    } else {
        destroyCorners();
        this->setZValue(2);
    }
    this->scene()->update();
}

bool GraphicsLine::isSelected() {
    if (itemInTree != nullptr)
        return itemInTree->isSelected();
    return false;
}

// create the corner grabbers
void GraphicsLine::createCorners() {
    m_outterborderColor = m_borderColor;

    for (int p=0; p<m_pointList.size(); p++) {
        if (m_corners[p] == nullptr) {
            m_corners[p] = new CornerGrabber(this,p);
            m_corners[p]->installSceneEventFilter(this);
        }
    }

    setCornerPositions();
}

void GraphicsLine::setCornerPositions() {
    if (m_corners[0] == nullptr || m_corners[1] == nullptr) return;
    int cornerWidth = (m_corners[0]->boundingRect().width())/2;
    int cornerHeight = ( m_corners[0]->boundingRect().height())/2;

    for (int p=0; p<m_pointList.size(); p++) {
        if (m_corners[p] != nullptr)
            m_corners[p]->setPos(m_pointList[p].x() - cornerWidth, m_pointList[p].y() - cornerHeight );
    }
}

#if 0
QRectF Transistion::boundingRect() const {
    return QRectF(0,0,_graphicsItemBoundingBoxWidth,_height);
}
#endif

void GraphicsLine::paint (QPainter *painter, const QStyleOptionGraphicsItem *i, QWidget *w) {
    Q_UNUSED(i);
    Q_UNUSED(w);

    if (getDrawingState()) {
        _pen.setStyle(borderStyle());
        _pen.setColor(m_borderColor);
        _pen.setWidth(getBorderWidth());
        painter->setPen(_pen);

        QBrush brush2(m_borderColor,Qt::SolidPattern);

        QPainterPath path;
        path.moveTo(m_pointList.at(0));

        for (int p=0; p<m_pointList.size(); p++)
            path.lineTo(m_pointList.at(p));

        painter->drawPath(path);

        /*_pen.setStyle(Qt::DotLine);
        _pen.setColor(Qt::red);
        painter->setPen(_pen);
        painter->drawPolygon(_selectRegion);*/

        // Draw the arrows
        static const double Pi = 3.14159265358979323846264338327950288419717;
        static double TwoPi = 2.0 * Pi;
        QLineF line(m_pointList[0], m_pointList[1]);
        double angle = ::acos(line.dx() / line.length());
        if (line.dy() >= 0)
             angle = TwoPi - angle;

        int arrowSize= 10;

        if (m_arrowStart) {
            QPointF sourceArrowP1 = m_pointList[0] + QPointF(sin(angle + Pi / 3) * arrowSize,
                                                          cos(angle + Pi / 3) * arrowSize);
            QPointF sourceArrowP2 = m_pointList[0] + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                                          cos(angle + Pi - Pi / 3) * arrowSize);
            QPointF mPoint = line.pointAt(5/line.length());

            QPolygonF polygon;
            polygon << m_pointList[0] << sourceArrowP1 << mPoint << sourceArrowP2;
            painter->drawPolygon(polygon);

            QPainterPath path;
            path.addPolygon(polygon);
            painter->fillPath(path,brush2);
        }
        if (m_arrowEnd) {
            QPointF destArrowP1 = m_pointList[1] + QPointF(sin(angle - Pi / 3) * arrowSize,
                                                      cos(angle - Pi / 3) * arrowSize);
            QPointF destArrowP2 = m_pointList[1] + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
                                                      cos(angle - Pi + Pi / 3) * arrowSize);

            QPointF mPoint = line.pointAt(1-5/line.length());

            QPolygonF polygon;
            polygon << m_pointList[1] << destArrowP1 << mPoint << destArrowP2;
            painter->drawPolygon(polygon);

            QPainterPath path;
            path.addPolygon(polygon);
            painter->fillPath(path,brush2);
        }
    }
}

void GraphicsLine::mouseMoveEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

void GraphicsLine::mousePressEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

QVariant GraphicsLine::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        GraphicsScene *model = qobject_cast<GraphicsScene *>(scene());
        if (model)
            model->itemMoving(this);
    }

    return QGraphicsItem::itemChange(change, value);
}

GraphicsLine *GraphicsLine::clone() {
    QPoint newPos(this->x(),this->y());
    newPos.setY(newPos.y()+5);
    newPos.setX(newPos.x()+5);

    auto newLine  = new GraphicsLine();
    newLine->setBorderColor(m_borderColor);
    newLine->setPos(newPos);
    newLine->setPointList(getPointList());
    newLine->setArrow(ArrowStart, this->getArrow(ArrowStart));
    newLine->setArrow(ArrowEnd, this->getArrow(ArrowEnd));
    newLine->setColorValue(BorderColor,this->getColorValue(BorderColor));
    newLine->setBorderWidth(this->getBorderWidth());
    return newLine;
}

void GraphicsLine::loadParamFromXML(QDomElement e) {
    GraphicsHelperClass::loadParamFromXML(e);

	m_pointList.clear();
    m_pointList << QPointF(e.attribute("lineStartX").toInt(), e.attribute("lineStartY").toInt()+20);
    m_pointList << QPointF(e.attribute("lineEndX").toInt(), e.attribute("lineEndY").toInt()+20);
    this->m_arrowStart = e.attribute("arrowStart","0").toInt();
    this->m_arrowEnd = e.attribute("arrowEnd","0").toInt();

    this->setZValue(2);
	initPolygon();
}

QDomElement GraphicsLine::saveParamToXML(QDomDocument *xmlDoc) {
    QDomElement elem = GraphicsHelperClass::saveParamToXML(xmlDoc);

    elem.setAttribute("lineStartX", (int)mapToParent(m_pointList[0]).x());
    elem.setAttribute("lineStartY", (int)mapToParent(m_pointList[0]).y()-20);
    elem.setAttribute("lineEndX", (int)mapToParent(m_pointList[1]).x());
    elem.setAttribute("lineEndY", (int)mapToParent(m_pointList[1]).y()-20);

    elem.setAttribute("arrowStart",this->m_arrowStart);
    elem.setAttribute("arrowEnd",this->m_arrowEnd);

    return elem;
}

void GraphicsLine::setArrow(QtRptName::Command command, QVariant value) {
    if (command == QtRptName::ArrowStart)
        m_arrowStart = value.toBool();
    if (command == QtRptName::ArrowEnd)
        m_arrowEnd = value.toBool();
    this->update();
}

bool GraphicsLine::getArrow(QtRptName::Command command) {
    bool result = false;
    if (command == QtRptName::ArrowStart)
        result = m_arrowStart;
    if (command == QtRptName::ArrowEnd)
        result = m_arrowEnd;
    return result;
}

qreal GraphicsLine::getLength() {
    QLineF line(m_pointList[0], m_pointList[1]);
    return line.length();
}

void GraphicsLine::setLength(qreal value) {
    QLineF line(m_pointList[0], m_pointList[1]);
    line.setLength(value);
    m_pointList[0] = line.p1();
    setEndPosition(line.p2());
}

void GraphicsLine::setMenu(QMenu *menu_) {
    QIcon icon;

    auto actContDel = new QAction(tr("Delete"),this);
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/delete.png")), QIcon::Normal, QIcon::On);
    actContDel->setObjectName("actContDel");
    actContDel->setIcon(icon);
    QObject::connect(actContDel, SIGNAL(triggered()), this, SLOT(deleteLater()));

    auto actContMoveForward = new QAction(tr("Move forward"),this);
    actContMoveForward->setObjectName("actContMoveForward");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/moveForward.png")), QIcon::Normal, QIcon::On);
    actContMoveForward->setIcon(icon);
    QObject::connect(actContMoveForward, SIGNAL(triggered()), this, SLOT(moveForward()));

    auto actContMoveBack = new QAction(tr("Move back"),this);
    actContMoveBack->setObjectName("actContMoveBack");
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/moveBack.png")), QIcon::Normal, QIcon::On);
    actContMoveBack->setIcon(icon);
    QObject::connect(actContMoveBack, SIGNAL(triggered()), this, SLOT(moveBack()));

    m_menu->clear();
    m_menu->insertActions(0,menu_->actions());
    m_menu->addAction(actContDel);
    m_menu->addSeparator();
    m_menu->addAction(actContMoveForward);
    m_menu->addAction(actContMoveBack);
}

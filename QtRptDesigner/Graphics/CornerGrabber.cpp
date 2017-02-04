#include "CornerGrabber.h"
#include <QApplication>
#include <QDebug>

CornerGrabber::CornerGrabber(QGraphicsItem *parent,  int corner) :
    QGraphicsItem(parent),
    mouseDownX(0),
    mouseDownY(0),
    outterborderColor_(Qt::black),
    outterborderPen_(),
    m_width(6),
    m_height(6),
    corner_(corner),
    mouseButtonState_(kMouseReleased)
{
    setParentItem(parent);

    outterborderPen_.setWidth(2);
    outterborderPen_.setColor(outterborderColor_);

   this->setAcceptHoverEvents(true);
}

void CornerGrabber::setMouseState(int s) {
    mouseButtonState_ = s;
}

int CornerGrabber::getMouseState() {
    return mouseButtonState_;
}

int CornerGrabber::getCorner() {
    return corner_;
}

// we have to implement the mouse events to keep the linker happy,
// but just set accepted to false since are not actually handling them
void CornerGrabber::mouseMoveEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

void CornerGrabber::mousePressEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

void CornerGrabber::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(true);
}

void CornerGrabber::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(false);
}

void CornerGrabber::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    event->setAccepted(false);
}

// change the color on hover events to indicate to the use the object has
// been captured by the mouse
void CornerGrabber::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ) {
    outterborderColor_ = Qt::black;

    // reset the cursor
    this->setCursorToResize(false);
    this->update(0,0,m_width,m_height);
}

void CornerGrabber::hoverEnterEvent ( QGraphicsSceneHoverEvent * ) {
    outterborderColor_ = Qt::red;

    // change the cursor to the diagonal sizer
    this->setCursorToResize(true);
    this->update(0,0,m_width,m_height);
}

QRectF CornerGrabber::boundingRect() const {
    return QRectF(0,0,m_width,m_height);
}

QPointF CornerGrabber::getCenterPoint() {
    return QPointF(pos().x() + m_width/2,pos().y() + m_height/2);
}

void CornerGrabber::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // fill the box with solid color, use sharp corners

    outterborderPen_.setCapStyle(Qt::SquareCap);
    outterborderPen_.setStyle(Qt::SolidLine);
    painter->setPen(outterborderPen_);

    QPointF topLeft (0, 0);
    QPointF bottomRight (m_width, m_height);

    QRectF rect (topLeft, bottomRight);

    QBrush brush (Qt::SolidPattern);
    brush.setColor (outterborderColor_);
    painter->fillRect(rect,brush);
}

void CornerGrabber::setCursorToResize(bool on) {
    if (on) {
        switch(corner_) {
        case 0:
        case 2:
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            break;
        case 1:
        case 3:
            QApplication::setOverrideCursor(Qt::SizeBDiagCursor);
            break;
        case 4:  //bottom center
        case 5:  //top center
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
            break;
        case 6:  //left center
        case 7:  //rigth center
            QApplication::setOverrideCursor(Qt::SizeHorCursor);
            break;
        }
    } else {
        QApplication::restoreOverrideCursor();
    }
}

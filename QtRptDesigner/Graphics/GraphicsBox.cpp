/*
Name: QtRpt
Version: 2.0.0
Web-site: http://www.qtrpt.tk
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
Web-site: http://www.aliks-os.tk

Copyright 2012-2016 Aleksey Osipov

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "GraphicsBox.h"
#include <QByteArray>
#include <QBrush>
#include <QLinearGradient>
#include <QApplication>
#include <QDebug>
#include "GraphicsScene.h"
#include <QAbstractTextDocumentLayout>
#include <QBuffer>

GraphicsBox::GraphicsBox():
        _outterborderColor(Qt::black),
        _outterborderPen()
{
    _location = QPointF(0,0);
    _dragStart = QPointF(0,0);
    _width = 200;
    _height = 20;

    _XcornerGrabBuffer = -3;
    _YcornerGrabBuffer = -3;
    _drawingWidth = (  _width -   _XcornerGrabBuffer);
    _drawingHeight = ( _height -  _YcornerGrabBuffer);
    _drawingOrigenX = ( _XcornerGrabBuffer);
    _drawingOrigenY = ( _YcornerGrabBuffer);

    for (unsigned int i=0; i<sizeof(_corners)/sizeof(*_corners); i++)
        _corners[i] = nullptr;

    setFlag(QGraphicsItem::ItemIsSelectable,true);
    setFlag(QGraphicsItem::ItemIsMovable,true);
    setFlag(ItemSendsGeometryChanges,true);

    m_autoHeight = false;
    m_borderIsVisible = true;
    m_backgroundColor = Qt::white;
    m_highlighting = "";
    m_formatString = "";
    m_text = tr("New Label");
    m_radius = 6;
    m_barcode = nullptr;
    m_crossTab = nullptr;
    m_chart = nullptr;

    if (QApplication::layoutDirection() == Qt::RightToLeft) {
        m_textDirection = true;
        m_alignment = Qt::AlignRight | Qt::AlignVCenter;
    } else {
        m_textDirection = false;
        m_alignment = Qt::AlignLeft | Qt::AlignVCenter;
    }

    _outterborderPen.setWidth(1);
    _outterborderPen.setColor(_outterborderColor);

    this->graphicsItem = this;
    this->adjustSize(0,0);
    this->setAcceptHoverEvents(true);
}

void GraphicsBox::setWidth(qreal value) {
    _width = value;
    adjustSize(0,0);
    setCornerPositions();
    if (this->scene() != nullptr)
        this->scene()->update();
}

void GraphicsBox::setHeight(qreal value) {
    _height = value;
    adjustSize(0,0);
    setCornerPositions();
    if (this->scene() != nullptr)
        this->scene()->update();
}

void GraphicsBox::adjustSize(int x, int y) {
    _width += x;
    _height += y;

    _drawingWidth  =  _width + _XcornerGrabBuffer;
    _drawingHeight =  _height + _YcornerGrabBuffer;

    if (m_chart != nullptr) {
        m_chart->resize(_width, _height);
    }
    if (m_crossTab != nullptr) {
        m_crossTab->rect.setHeight(_height);
        m_crossTab->rect.setWidth(_width);
    }
}

bool GraphicsBox::sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) {
    //qDebug() << " QEvent == " + QString::number(event->type());

    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL) {
        // this is not one of the mouse events we are interrested in
        return false;
    }

    switch (event->type() ) {
        // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress: {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();

                //Посылаем сигнал в сцену для отслеживания Ундо при перемещении концов
                GraphicsScene *model = qobject_cast<GraphicsScene *>(scene());
                if (model) {
                    model->itemMoving(this);
                }
            }
            break;
        case QEvent::GraphicsSceneMouseRelease: {
                corner->setMouseState(CornerGrabber::kMouseReleased);
            }
            break;
        case QEvent::GraphicsSceneMouseMove: {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;
        default:
            // we dont care about the rest of the events
            return false;
            break;
    }

    if (corner->getMouseState() == CornerGrabber::kMouseMoving ) {
        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.
        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() ) {
        case 0:
            XaxisSign = +1;
            YaxisSign = +1;
            break;
        case 1:
            XaxisSign = -1;
            YaxisSign = +1;
            break;
        case 2:
            if (type() == ItemType::GBand) XaxisSign = 0;
            else XaxisSign = -1;
            YaxisSign = -1;
            break;
        case 3:
            if (type() == ItemType::GBand) XaxisSign = 0;
            else XaxisSign = +1;
            YaxisSign = -1;
            break;
        case 4:  //bottom-center
            XaxisSign = 0;
            YaxisSign = -1;
            break;
        case 5:  //top-center
            XaxisSign = 0;
            YaxisSign = +1;
            break;
        case 6:  //left-center
            XaxisSign = +1;
            YaxisSign = 0;
            break;
        case 7:  //right-center
            XaxisSign = -1;
            YaxisSign = 0;
            break;
        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = _width + ( XaxisSign * xMoved);
        int newHeight = _height + (YaxisSign * yMoved) ;

        int deltaWidth  =   newWidth - _width ;
        int deltaHeight =   newHeight - _height ;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == 0 ) {
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);
        }
        if ( corner->getCorner() == 1 ) {
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        if ( corner->getCorner() == 3 ) {
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());
        }
        if ( corner->getCorner() == 5 ) {  //top-center
            int newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);
        }
        if ( corner->getCorner() == 6 ) {  //left-center
            int newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos, this->pos().y());
        }
        GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
        m_scene->itemResizing(this);
        setCornerPositions();
        this->scene()->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}

QPointF GraphicsBox::getPos() {
    return _location;
}

void GraphicsBox::setPos(QPointF pos) {
    _location = pos;
    QGraphicsItem::setPos(pos);
}

void GraphicsBox::setPos(qreal x, qreal y) {
    setPos(QPoint(x,y));
}

void GraphicsBox::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ) {
    this->scene()->update();
}

void GraphicsBox::hoverEnterEvent ( QGraphicsSceneHoverEvent * ) {
    this->scene()->update();
}

//for supporting moving the box across the scene
void GraphicsBox::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mouseReleaseEvent(event);
    event->setAccepted(true);
}

// for supporting moving the box across the scene
void GraphicsBox::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mousePressEvent(event);
    event->setAccepted(true);
    _dragStart = event->pos();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setSelected(!this->isSelected());
}

// for supporting moving the box across the scene
void GraphicsBox::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    if (this->type() == ItemType::GBand) return;
    QGraphicsItem::mouseMoveEvent(event); // move the item...
    GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());

    ReportBand *band = static_cast<ReportBand*>(this->parentItem());
    if (band != nullptr) {
        bool toBound = false;
        if (x() <= 0) {
            setPos(0, y());
            toBound = true;
        } else if (x() > band->getWidth()-this->getWidth()) {
            setPos(band->getWidth()-this->getWidth(), y());
            toBound = true;
        }

        if (y() <= band->titleHeight) {
            setPos(x(), band->titleHeight);
            toBound = true;
        } else if (y() > band->getHeight()-this->getHeight()) {
            setPos(x(), band->getHeight()-this->getHeight());
            toBound = true;
        }
        m_scene->itemResizing(this);
        if (toBound == true) return;
    }

    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);
    this->setPos(_location);
    this->scene()->update();
}

void GraphicsBox::setSelected(bool selected_) {
    QGraphicsItem::setSelected(selected_);
    if (itemInTree != nullptr)
        itemInTree->setSelected(selected_);
    if (selected_) {
        createCorners();
        GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
        emit m_scene->itemSelected(this);
    } else
        destroyCorners();
    this->scene()->update();
}

bool GraphicsBox::isSelected() {
    if (itemInTree != nullptr)
        return itemInTree->isSelected();
    return false;
}

// create the corner grabbers
void GraphicsBox::createCorners() {
    _outterborderColor = m_borderColor;

    if (type() != ItemType::GBand) {
        if (_corners[0] == nullptr) {
            _corners[0] = new CornerGrabber(this,0);
            _corners[0]->installSceneEventFilter(this);
        }
        if (_corners[1] == nullptr) {
            _corners[1] = new CornerGrabber(this,1);
            _corners[1]->installSceneEventFilter(this);
        }
        if (_corners[5] == nullptr) {  //top-center
            _corners[5] = new CornerGrabber(this,5);
            _corners[5]->installSceneEventFilter(this);
        }
        if (_corners[6] == nullptr) {  //left-center
            _corners[6] = new CornerGrabber(this,6);
            _corners[6]->installSceneEventFilter(this);
        }
        if (_corners[7] == nullptr) {  //rigth-center
            _corners[7] = new CornerGrabber(this,7);
            _corners[7]->installSceneEventFilter(this);
        }
    }
    if (_corners[2] == nullptr) {
        _corners[2] = new CornerGrabber(this,2);
        _corners[2]->installSceneEventFilter(this);
    }
    if (_corners[3] == nullptr) {
        _corners[3] = new CornerGrabber(this,3);
        _corners[3]->installSceneEventFilter(this);
    }
    if (_corners[4] == nullptr) {  //bottom-center
        _corners[4] = new CornerGrabber(this,4);
        _corners[4]->installSceneEventFilter(this);
    }
    setCornerPositions();
}

// remove the corner grabbers
void GraphicsBox::destroyCorners() {
    _outterborderColor = m_borderColor;

    for (unsigned int i=0; i<sizeof(_corners)/sizeof(*_corners); i++) {
        if (_corners[i] != nullptr) {
            _corners[i]->setParentItem(NULL);
            delete _corners[i];
            _corners[i] = nullptr;
        }
    }
}

void GraphicsBox::setCornerPositions() {
    if (_corners[0] != nullptr)  //top-left
        _corners[0]->setPos(_drawingOrigenX, _drawingOrigenY);
    if (_corners[1] != nullptr)
        _corners[1]->setPos(_drawingWidth,  _drawingOrigenY);
    if (_corners[2] != nullptr)
        _corners[2]->setPos(_drawingWidth , _drawingHeight);
    if (_corners[3] != nullptr)
        _corners[3]->setPos(_drawingOrigenX, _drawingHeight);
    if (_corners[4] != nullptr) //bottom-center
        _corners[4]->setPos((_drawingWidth-_drawingOrigenX)/2, _drawingHeight);
    if (_corners[5] != nullptr) //top-center
        _corners[5]->setPos((_drawingWidth-_drawingOrigenX)/2, _drawingOrigenY);
    if (_corners[6] != nullptr) //left-center
        _corners[6]->setPos(_drawingOrigenX, (_drawingHeight+_drawingOrigenY)/2);
    if (_corners[7] != nullptr) //rigth-center
        _corners[7]->setPos(_drawingWidth, (_drawingHeight+_drawingOrigenY)/2);
}

QRectF GraphicsBox::boundingRect() const {
    return QRectF(0,0,_width-1,_height);
}

void GraphicsBox::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QBrush brush2(m_backgroundColor,Qt::SolidPattern);

    _outterborderPen.setCapStyle(Qt::RoundCap);
    _outterborderPen.setStyle(borderStyle());
    _outterborderPen.setWidth(getBorderWidth());

    if (m_borderIsVisible)
        _outterborderPen.setColor( m_borderColor );
    else
        _outterborderPen.setColor(QColor(0,0,0,0));

    painter->setPen(_outterborderPen);

    QRectF rcT (QPointF(2,0), QPointF(getWidth(), getHeight()));

    if (type() == ItemType::GBand) {
        QRectF rc (QPointF(0,0), QPointF(getWidth()-1, getHeight()));
        painter->drawRect(rc);

        QPointF p2R(getWidth(),20);
        QRectF textRect(QPointF(0,0), p2R);
        QRectF fillRect(QPointF(1,1), QPointF(p2R.x()-1,p2R.y()));
        painter->fillRect(fillRect, QBrush(QColor(232,183,98,255)));

        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        painter->drawLine(QPointF(0,20),  p2R);
        painter->drawText(textRect,Qt::AlignCenter,m_text);
        painter->drawPixmap(QRect(_drawingWidth-18,2,16,16), m_bandPixmap);
    }
    if (type() == ItemType::GBox) {
        QRectF rc (QPointF(0,0), QPointF(getWidth(), getHeight()));
        switch(this->getFieldType()) {
            case Text:
            case TextImage:
                {
                if (m_backgroundColor != Qt::white)
                    painter->fillRect(rc, brush2);

                if (borderIsCheck(FrameTop))
                    painter->drawLine(0,0, getWidth()-1,0);  //top
                if (borderIsCheck(FrameLeft))
                    painter->drawLine(0,0, 0,getHeight()); //left
                if (borderIsCheck(FrameRight))
                    painter->drawLine(getWidth()-1,0, getWidth()-1, getHeight()); //right
                if (borderIsCheck(FrameBottom))
                    painter->drawLine(0,getHeight(), getWidth()-1, getHeight()); //bottom

                _outterborderPen.setColor( m_fontColor );
                painter->setPen(_outterborderPen);

                painter->setFont(m_font);
                painter->drawText(rcT,m_alignment,m_text);
                break;
            }
            case TextRich: {
                QTextDocument td;
                td.setHtml(m_text);
                QAbstractTextDocumentLayout::PaintContext ctx;
                ctx.clip = QRectF( 0, 0, getWidth(), getHeight() );
                td.documentLayout()->draw( painter, ctx );
                break;
            }
            case Image: {
                Qt::AspectRatioMode aspect = Qt::KeepAspectRatio;
                if (m_ignoreAspectRatio)
                    aspect = Qt::IgnoreAspectRatio;

                QSize pixSize = m_pixmap.size();
                pixSize.scale(rc.size().width(), rc.size().height(), aspect);

                QPixmap scaledPix = m_pixmap.scaled(pixSize, aspect, Qt::SmoothTransformation );
                painter->drawPixmap(0, 0,
                                    (int)pixSize.width(),
                                    (int)pixSize.height(), scaledPix);
                break;
            }
            case Circle: {
                painter->setBrush(brush2);
                painter->drawEllipse(0, 0, getWidth() - 1, getHeight() - 1);
                break;
            }
            case Triangle: {
                qreal pointX1 = 0;
                qreal pointY1 = getHeight()-1;

                qreal pointX2 = getWidth();
                qreal pointY2 = getHeight()-1;

                qreal pointX3 = getWidth()/2;
                qreal pointY3 = 0;

                QPainterPath path;
                path.moveTo (pointX1, pointY1);
                path.lineTo (pointX2, pointY2);
                path.lineTo (pointX3, pointY3);
                path.lineTo (pointX1, pointY1);
                painter->fillPath (path, brush2);
                painter->drawPath(path);
                break;
            }
            case Rhombus: {
                qreal pointX1 = getWidth()/2;
                qreal pointY1 = getHeight()-1;

                qreal pointX2 = getWidth();
                qreal pointY2 = getHeight()/2;

                qreal pointX3 = getWidth()/2;
                qreal pointY3 = 1;

                qreal pointX4 = 1;
                qreal pointY4 = getHeight()/2;

                QPainterPath path;
                path.moveTo (pointX1, pointY1);
                path.lineTo (pointX2, pointY2);
                path.lineTo (pointX3, pointY3);
                path.lineTo (pointX4, pointY4);
                path.lineTo (pointX1, pointY1);
                painter->fillPath (path, brush2);
                painter->drawPath(path);
                break;
            }
            case RoundedReactangle: {
                QRectF rect(1,1,getWidth()-2,getHeight()-2);
                painter->setBrush(brush2);
                painter->drawRoundedRect(rect, m_radius, m_radius);
                break;
            }
            case Reactangle: {
                QRectF rect(1,1,getWidth()-2,getHeight()-2);
                painter->setBrush(brush2);
                painter->drawRect(rect);
                break;
            }
            case Barcode: {
                if (m_barcode != nullptr) {
                    m_barcode->setValue(m_text);
                    m_barcode->drawBarcode(painter,0,0,this->getWidth(),this->getHeight());
                }
                break;
            }
            case Diagram: {
                if (m_chart != nullptr) {
                    m_chart->paintChart(painter);
                }
                break;
            }
            case CrossTab: {
                painter->setPen(QPen( getColorValue(BorderColor), 1, Qt::SolidLine, Qt::RoundCap));

                int fieldWidth = m_crossTab->rect.width()/m_crossTab->colCount();
                int fieldheight = m_crossTab->rowHeight(); //m_crossTab->rect.height()/m_crossTab->rowCount();
                int posInCell_V = fieldheight/2+5;
                int posInCell_H = 5;

                QVarLengthArray<QLineF, 100> lines;
                painter->drawRect(0, 0,
                                  m_crossTab->rect.width(),
                                  m_crossTab->rect.height());

                painter->setPen(QPen( getColorValue(BorderColor), 1, Qt::DashLine, Qt::RoundCap));

                //vertical lines
                for (qreal x = 0; x <= m_crossTab->rect.right(); x += fieldWidth)
                    lines.append(QLineF(x, 0,
                                        x, m_crossTab->rect.bottom()));

                //horizontal lines
                for (qreal y = 0; y <= m_crossTab->rect.bottom(); y += fieldheight)
                    lines.append(QLineF(m_crossTab->rect.left(), y,
                                        m_crossTab->rect.right(), y));

                painter->drawLines(lines.data(), lines.size());

                painter->setPen(QPen( getColorValue(BorderColor), 1, Qt::SolidLine, Qt::RoundCap));



                /*for (int col=0; col<m_crossTab->colCount()+1; col++) {
                    QPoint p1(col*fieldWidth, 0),
                           p2(col*fieldWidth, this->getHeight());
                    if (col != m_crossTab->colCount() )
                        painter->drawLine(p1,p2);
                    else {
                        QPoint p1(this->getWidth(), 0),
                               p2(this->getWidth(), this->getHeight());
                        painter->drawLine(p1,p2);
                    }

                    if (m_crossTab->isColHeaderVisible()) {
                        if (col<m_crossTab->colDataCount()) {
                            int tmpCol = col;
                            if (m_crossTab->isRowHeaderVisible()) {
                                tmpCol += 1;
                            }

                            QString col_txt = m_crossTab->getColName(col);
                            QPoint p1(tmpCol*fieldWidth+posInCell_H, posInCell_V);
                            painter->drawText(p1,col_txt);
                        }

                        //Col "total" - total per row
                        if (m_crossTab->isRowTotalVisible()) {
                            if (col == m_crossTab->colCount()) {
                                QPoint p1((m_crossTab->colCount()-1) * fieldWidth+posInCell_H, posInCell_V);
                                painter->drawText(p1,tr("Total"));
                            }
                        }
                    }
                }*/

                break;
            }
            default: {

            }
        }
    }
}

void GraphicsBox::mouseMoveEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

void GraphicsBox::mousePressEvent(QGraphicsSceneDragDropEvent *event) {
    event->setAccepted(false);
}

void GraphicsBox::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    edit();
    //QGraphicsItem::mouseDoubleClickEvent(event);
}

QVariant GraphicsBox::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        GraphicsScene *m_scene = qobject_cast<GraphicsScene *>(scene());
        if (m_scene) {
            m_scene->itemMoving(this);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

GraphicsBox* GraphicsBox::clone() {
    QPoint newPos(this->x(),this->y());
    newPos.setY(newPos.y()+5);
    newPos.setX(newPos.x()+5);

    GraphicsBox *newContField  = new GraphicsBox();
    newContField->setColorValue(FrameTop, this->getColorValue(FrameTop));
    newContField->setColorValue(FrameBottom, this->getColorValue(FrameBottom));
    newContField->setColorValue(FrameLeft, this->getColorValue(FrameLeft));
    newContField->setColorValue(FrameRight, this->getColorValue(FrameRight));
    newContField->setFieldType(this->getFieldType());
    newContField->setText(this->getText());
    newContField->setFont(this->getFont());
    newContField->setAlignment(this->getAlignment());
    newContField->setFont(this->getFont());
    newContField->setWidth(this->getWidth());
    newContField->setHeight(this->getHeight());
    newContField->m_formatString = this->m_formatString;
    newContField->m_textWrap = this->m_textWrap;
    if (newContField->getFieldType() == Image) {
        newContField->setImgFromat(this->getImgFormat());
        newContField->setIgnoreAspectRatio(this->getIgnoreAspectRatio());
        newContField->setImage(this->getImage());
    }
    if (newContField->getFieldType() == Barcode) {
        newContField->setBarcodeType(this->getBarcodeType());
        newContField->setBarcodeFrameType(this->getBarcodeFrameType());
    }
    if (newContField->getFieldType() == CrossTab) {
        newContField->m_crossTab = this->m_crossTab;
    }
    newContField->setVisible(true);
    newContField->setPos(newPos);
    return newContField;
}

void GraphicsBox::loadParamFromXML(QDomElement e) {
    GraphicsHelperClass::loadParamFromXML(e);
    this->setFieldType(m_type);

    this->setPos(e.attribute("left").toInt(), e.attribute("top").toInt()+20);
    this->setWidth(e.attribute("width").toInt());
    this->setHeight(e.attribute("height").toInt());

    if (this->m_type == Text) {
        this->m_formatString = e.attribute("format","");
        this->m_highlighting = e.attribute("highlighting","");
        m_textWrap = e.attribute("textWrap","1").toInt();
    } else if (this->m_type == Image || e.attribute("picture","text") != "text") {
        //load picture into lable
        QByteArray byteArray = QByteArray::fromBase64(e.attribute("picture","text").toLatin1());
        m_imgFormat = e.attribute("imgFormat","PNG");
        m_pixmap = QPixmap::fromImage(QImage::fromData(byteArray, m_imgFormat.toLatin1().data()));
        m_ignoreAspectRatio = e.attribute("ignoreAspectRatio","1").toInt();
    } else if (this->m_type == Diagram) {
        m_chart->setParams(e.attribute("showGrid","1").toInt(),
                         e.attribute("showLegend","1").toInt(),
                         e.attribute("showCaption","1").toInt(),
                         e.attribute("showGraphCaption","1").toInt(),
                         e.attribute("showPercent","1").toInt(),
                         e.attribute("caption","Example"),
                         e.attribute("autoFillData","0").toInt()
                         );
        m_chart->loadXML(e);
    } else if (this->m_type == Barcode) {
        setBarcodeType( (BarCode::BarcodeTypes)e.attribute("barcodeType","13").toInt() );
        setBarcodeFrameType( (BarCode::FrameTypes)e.attribute("barcodeFrameType","0").toInt() );
        setBarcodeHeight(e.attribute("barcodeHeight","50").toInt() );
    } else if (this->m_type == CrossTab) {
        m_crossTab->setRowHeight(e.attribute("rowHeight","20").toInt());

        m_crossTab->setColHeaderVisible(e.attribute("crossTabColHeaderVisible","1").toInt());
        m_crossTab->setRowHeaderVisible(e.attribute("crossTabRowHeaderVisible","1").toInt());
        m_crossTab->setColTotalVisible(e.attribute("crossTabColTotalVisible","1").toInt());
        m_crossTab->setRowTotalVisible(e.attribute("crossTabRowTotalVisible","1").toInt());
        m_crossTab->clear();
        QDomNode g = e.firstChild();
        while(!g.isNull()) {
            QDomElement ge = g.toElement(); // try to convert the node to an element.
            if (ge.nodeName() == "row") {
                m_crossTab->addRow(ge.attribute("caption"));
            }
            if (ge.nodeName() == "col") {
                m_crossTab->addCol(ge.attribute("caption"));
            }
            g = g.nextSibling();
        }
        m_crossTab->initMatrix();
    }
    m_text = e.attribute("value");

    if (this->m_type != CrossTab)
        this->setColorValue(BackgroundColor, colorFromString(e.attribute("backgroundColor","rgba(255,255,255,0)")));
    this->setColorValue(FontColor, colorFromString(e.attribute("fontColor","rgba(0,0,0,255)")));
    this->setColorValue(FrameTop, colorFromString(e.attribute("borderTop","rgba(0,0,0,255)")));
    this->setColorValue(FrameBottom, colorFromString(e.attribute("borderBottom","rgba(0,0,0,255)")));
    this->setColorValue(FrameLeft, colorFromString(e.attribute("borderLeft","rgba(0,0,0,255)")));
    this->setColorValue(FrameRight, colorFromString(e.attribute("borderRight","rgba(0,0,0,255)")));

    m_font.setBold(e.attribute("fontBold").toInt());
    m_font.setItalic(e.attribute("fontItalic").toInt());
    m_font.setUnderline(e.attribute("fontUnderline").toInt());
    m_font.setStrikeOut(e.attribute("fontStrikeout").toInt());
    m_font.setFamily(e.attribute("fontFamily"));
    int fntSize = e.attribute("fontSize","8").toInt();
    if (fntSize == 0) fntSize = 8;
    m_font.setPointSize(fntSize);
    this->m_autoHeight = e.attribute("autoHeight","0").toInt();

    Qt::Alignment hAl, vAl;
    if (e.attribute("aligmentH")== "hLeft")
        hAl = Qt::AlignLeft;
    else if (e.attribute("aligmentH")== "hRight")
        hAl = Qt::AlignRight;
    else if (e.attribute("aligmentH")== "hCenter")
        hAl = Qt::AlignHCenter;
    else if (e.attribute("aligmentH") == "hJustify")
        hAl = Qt::AlignJustify;
    else
        hAl = Qt::AlignLeft;

    if (e.attribute("aligmentV") == "vTop")
        vAl = Qt::AlignTop;
    else if (e.attribute("aligmentV") == "vBottom")
        vAl = Qt::AlignBottom;
    else if (e.attribute("aligmentV") == "vCenter")
        vAl = Qt::AlignVCenter;
    else
        vAl = Qt::AlignVCenter;

    m_alignment = hAl | vAl;
}

QDomElement GraphicsBox::saveParamToXML(QDomDocument *xmlDoc) {
    QDomElement elem = GraphicsHelperClass::saveParamToXML(xmlDoc);

    elem.setAttribute("top",this->_location.y()-20);
    elem.setAttribute("left",this->_location.x());
    elem.setAttribute("width",this->_width);
    elem.setAttribute("height",this->_height);

    //---FROM TCONTAINERFIELD
    if (this->m_type == Text) {
        elem.setAttribute("format",this->m_formatString);
        elem.setAttribute("highlighting",this->m_highlighting);
        elem.setAttribute("textWrap",this->m_textWrap);
    }
    if (this->m_type == Image) {
        //Saving picture
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);

        if (m_imgFormat.isEmpty() || m_imgFormat.isNull())
            m_pixmap.save(&buffer, "PNG");
        else
            m_pixmap.save(&buffer, m_imgFormat.toLatin1().data());
        QString s = byteArray.toBase64();
        elem.setAttribute("picture",s);
        elem.setAttribute("imgFormat",m_imgFormat);
        elem.setAttribute("ignoreAspectRatio",m_ignoreAspectRatio);
    }
    if (this->m_type == Diagram) {
        elem.setAttribute("showGrid",m_chart->getParam(DrawGrid).toBool());
        elem.setAttribute("showLegend",m_chart->getParam(ShowLegend).toBool());
        elem.setAttribute("showCaption",m_chart->getParam(ShowCaption).toBool());
        elem.setAttribute("showGraphCaption",m_chart->getParam(ShowGraphCaption).toBool());
        elem.setAttribute("showPercent",m_chart->getParam(ShowPercent).toBool());
        elem.setAttribute("caption",m_chart->getParam(Caption).toString());
        elem.setAttribute("autoFillData",m_chart->getParam(AutoFillData).toBool());

        if (m_chart->getParam(AutoFillData).toBool()) {
            //get info about graphs
            for(auto graphParam : getChart()->getGraphParamList()) {
                QDomElement graph = xmlDoc->createElement("graph");
                graph.setAttribute("caption",graphParam.caption);
                graph.setAttribute("value",graphParam.valueString);
                graph.setAttribute("color",colorToString(graphParam.color));
                elem.appendChild(graph);
            }
        }
    }
    if (this->m_type == Barcode) {
        elem.setAttribute("barcodeType",m_barcode->getBarcodeType());
        elem.setAttribute("barcodeFrameType",m_barcode->getFrameType());
        elem.setAttribute("barcodeHeight",m_barcode->getHeight());
    }
    if (this->m_type == CrossTab) {
        elem.setAttribute("rowHeight",m_crossTab->rowHeight());

        elem.setAttribute("crossTabColHeaderVisible",m_crossTab->isColHeaderVisible());
        elem.setAttribute("crossTabRowHeaderVisible",m_crossTab->isRowHeaderVisible());
        elem.setAttribute("crossTabColTotalVisible",m_crossTab->isColTotalVisible());
        elem.setAttribute("crossTabRowTotalVisible",m_crossTab->isRowTotalVisible());
        for(int i=0; i<m_crossTab->rowDataCount(); i++) {
            QDomElement row = xmlDoc->createElement("row");
            row.setAttribute("caption",m_crossTab->getRowName(i));
            elem.appendChild(row);
        }
        for(int i=0; i<m_crossTab->colDataCount(); i++) {
            QDomElement col = xmlDoc->createElement("col");
            col.setAttribute("caption",m_crossTab->getColName(i));
            elem.appendChild(col);
        }
    }

    QString hAl, vAl;
    if (getAlignment() & Qt::AlignLeft)
        hAl = "hLeft";
    else if (getAlignment() & Qt::AlignRight)
        hAl = "hRight";
    else if (getAlignment() & Qt::AlignHCenter)
        hAl = "hCenter";
    else if (getAlignment() & Qt::AlignJustify)
        hAl = "hJustify";
    else
        hAl = "hLeft";

    if (getAlignment() & Qt::AlignTop)
        vAl = "vTop";
    else if (getAlignment() & Qt::AlignBottom)
        vAl = "vBottom";
    else if (getAlignment() & Qt::AlignVCenter)
        vAl = "vCenter";
    else
        vAl = "vCenter";

    elem.setAttribute("aligmentH",hAl);
    elem.setAttribute("aligmentV",vAl);
    elem.setAttribute("value",m_text);

    elem.setAttribute("fontBold",m_font.bold());
    elem.setAttribute("fontItalic",m_font.italic());
    elem.setAttribute("fontUnderline",m_font.underline());
    elem.setAttribute("fontStrikeout",m_font.strikeOut());
    if (m_font.family().isEmpty()) elem.setAttribute("fontFamily","Arial");
    else elem.setAttribute("fontFamily",m_font.family());
    elem.setAttribute("fontSize",m_font.pointSize());

    QString fontColor = colorToString(getColorValue(FontColor));
    QString backgroundColor = colorToString(getColorValue(BackgroundColor));
    QString top = colorToString(getColorValue(FrameTop));
    QString bottom = colorToString(getColorValue(FrameBottom));
    QString left = colorToString(getColorValue(FrameLeft));
    QString right = colorToString(getColorValue(FrameRight));

    elem.setAttribute("fontColor",fontColor);
    elem.setAttribute("backgroundColor",backgroundColor);
    elem.setAttribute("borderTop",top);
    elem.setAttribute("borderBottom",bottom);
    elem.setAttribute("borderLeft",left);
    elem.setAttribute("borderRight",right);

    elem.setAttribute("autoHeight",this->m_autoHeight);

    return elem;
}

void GraphicsBox::setFieldType(FieldType value) {
    GraphicsHelperClass::setFieldType(value);
    switch(value) {
        case TextRich: {
            setAlignment(Qt::AlignTop);
            break;
        }
        case Reactangle:
        case RoundedReactangle:
        case Rhombus:
        case Triangle:
        case Circle: {
            break;
        }
        case Image: {
            setText(tr("New image"));
            setIgnoreAspectRatio(true);
            setImage(QPixmap(QString::fromUtf8(":/new/prefix1/images/picture.png")));
            break;
        }
        case Diagram: {
            setText(tr("New diagram"));
            setIgnoreAspectRatio(true);
            setImage(QPixmap(QString::fromUtf8(":/new/prefix1/images/diagram.png")));

            this->setWidth(300);
            this->setHeight(300);

            m_chart = new Chart(0);
            m_chart->setObjectName("chart");
            m_chart->setVisible(false);
            m_chart->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            m_chart->resize(_width, _height);
            //if (xmlDoc != 0)
            //    m_chart->loadXML(xmlDoc->createElement("TContainerField"));
            break;
        }
        case Barcode: {
            m_barcode = new BarCode(0);
            this->setWidth(200);
            this->setHeight(100);
            break;
        }
        case CrossTab: {
            this->setWidth(400);
            this->setHeight(300);
            m_crossTab = new RptCrossTabObject();
            m_crossTab->rect.setHeight(this->getHeight());
            m_crossTab->rect.setWidth(this->getWidth());
            m_crossTab->addCol("C1");
            m_crossTab->addCol("C2");
            m_crossTab->addCol("C3");
            m_crossTab->addRow("R1");
            m_crossTab->addRow("R2");
            m_crossTab->addRow("R3");
            m_crossTab->setColHeaderVisible(true);
            m_crossTab->setRowHeaderVisible(true);
            m_crossTab->setColTotalVisible(true);
            m_crossTab->setRowTotalVisible(true);
            m_crossTab->initMatrix();
            //Fill values into matrix
            for (int r=0; r<m_crossTab->rowDataCount(); r++)
                for (int c=0; c<m_crossTab->colDataCount(); c++)
                    m_crossTab->setMatrixValue(QString::number(c),
                                               QString::number(r),
                                               QString("%1%2").arg(c).arg(r).toDouble());
            break;
        }
        default:
            break;
    }
    graphicsItem->setZValue(11);
}

void GraphicsBox::setText(QString value) {
    m_text = value;
}

QString GraphicsBox::getText() {
    return m_text;
}

void GraphicsBox::setBorderVisible(bool value) {
    m_borderIsVisible = value;
}

bool GraphicsBox::borderIsVisible() {
    return m_borderIsVisible;
}

void GraphicsBox::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    m_menu->popup(event->screenPos());
}

void GraphicsBox::setMenu(QMenu *menu_) {
    if (this->type() == ItemType::GBand) return;

    QIcon icon;
    auto actContEdit = new QAction(tr("Edit"),this);
    actContEdit->setObjectName("actContEdit");
    QObject::connect(actContEdit, SIGNAL(triggered()), this, SLOT(edit()));

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
    m_menu->addAction(actContEdit);
    m_menu->addAction(actContDel);
    m_menu->addSeparator();
    m_menu->addAction(actContMoveForward);
    m_menu->addAction(actContMoveBack);
}

BarCode::BarcodeTypes GraphicsBox::getBarcodeType() {
    return m_barcode->getBarcodeType();
}

void GraphicsBox::setBarcodeType(BarCode::BarcodeTypes value) {
    m_barcode->setBarcodeType(value);
    update();
}

BarCode::FrameTypes GraphicsBox::getBarcodeFrameType() {
    return m_barcode->getFrameType();
}

void GraphicsBox::setBarcodeFrameType(BarCode::FrameTypes value) {
    m_barcode->setFrameType(value);
    update();
}

int GraphicsBox::getBarcodeHeight() {
    return m_barcode->getHeight();
}

void GraphicsBox::setBarcodeHeight(int value) {
    m_barcode->setHeight(value);
    update();
}

void GraphicsBox::setImage(QPixmap p) {
    m_pixmap = p;
}

QPixmap GraphicsBox::getImage() {
    return m_pixmap;
}

QString GraphicsBox::getImgFormat() {
    return m_imgFormat;
}

void GraphicsBox::setImgFromat(QString value) {
    m_imgFormat = value;
}

RptCrossTabObject *GraphicsBox::getCrossTab() {
    return m_crossTab;
}

Chart *GraphicsBox::getChart() {
    return m_chart;
}

BarCode *GraphicsBox::getBarCode() {
    return m_barcode;
}

GraphicsBox::~GraphicsBox() {
    if (m_barcode != nullptr)
        delete m_barcode;
    if (m_crossTab != nullptr)
        delete m_crossTab;
    if (m_chart != nullptr)
        delete m_chart;
}

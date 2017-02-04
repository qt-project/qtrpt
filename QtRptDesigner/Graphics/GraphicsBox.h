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

#ifndef GRAPHICSBOX_H
#define GRAPHICSBOX_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include "CornerGrabber.h"
#include "GraphicsHelperClass.h"
#include <QTextDocument>
#include <QObject>
#include <QDomDocument>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <Barcode.h>

class GraphicsBox : public QGraphicsItem, public GraphicsHelperClass
{
public:
    GraphicsBox();
    ~GraphicsBox();
    virtual void setPos(QPointF pos);
    virtual void setPos(qreal x, qreal y);
    QPointF getPos();
    int type() const Q_DECL_OVERRIDE { return ItemType::GBox; }
    void setSelected(bool selected_);
    bool isSelected();
    void setWidth(qreal value);
    void setHeight(qreal value);
    qreal getWidth() {return _width;}
    qreal getHeight() {return _height;}
    GraphicsBox *clone();
    void loadParamFromXML(QDomElement e);
    QDomElement saveParamToXML(QDomDocument *xmlDoc);
    void setText(QString value); //
    QString getText(); //
    void setFont(QFont font) {m_font = font;}
    QFont getFont() {return m_font;}
    Qt::Alignment getAlignment() {return m_alignment;}
    void setAlignment(Qt::Alignment value) {m_alignment = value;}
    void setBandPixmap(QPixmap pixmap) {m_bandPixmap = pixmap;}
    void setBorderVisible(bool value);
    bool borderIsVisible();
    bool getIgnoreAspectRatio() {return m_ignoreAspectRatio;}
    void setIgnoreAspectRatio(bool value) {m_ignoreAspectRatio = value;}
    bool getAutoHeight() {return m_autoHeight;}
    void setAutoHeight(bool value) {m_autoHeight = value;}
    void setTextDirection(bool value) {m_textDirection = value;}
    bool getTextDirection() {return m_textDirection;}
    void setTextWrap(bool value) {m_textWrap = value;}
    bool getTextWrap() {return m_textWrap;}
    QString getFormatString() {return m_formatString;}
    void setFormatString(QString value) {m_formatString = value;}
    void setImage(QPixmap p);
    QPixmap getImage();
    QString getImgFormat();
    void setImgFromat(QString value);
    void setMenu(QMenu *menu_);
    BarCode::BarcodeTypes getBarcodeType();
    void setBarcodeType(BarCode::BarcodeTypes value);
    BarCode::FrameTypes getBarcodeFrameType();
    void setBarcodeFrameType(BarCode::FrameTypes value);
    int getBarcodeHeight();
    void setBarcodeHeight(int value);
    void setFieldType(FieldType value);
    QString getHighlighting() {return m_highlighting;}
    void setHighlighting(QString value) {m_highlighting = value;}
    RptCrossTabObject *getCrossTab();
    Chart *getChart();
    BarCode *getBarCode();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    qreal   _drawingWidth;
    qreal   _drawingHeight;
    qreal   _drawingOrigenX;
    qreal   _drawingOrigenY;

    void createCorners();
    void setCornerPositions();
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter ( QGraphicsItem * watched, QEvent * event ) ;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void adjustSize(int x, int y);

    QPen _outterborderPen; ///< the pen is used to paint the red/black border
    QPointF _location;
    QPointF _dragStart;
    qreal   _width;
    qreal   _height;

    int _XcornerGrabBuffer;
    int _YcornerGrabBuffer;

    QString m_text;
    bool m_borderIsVisible;

    QPixmap m_bandPixmap;
    QFont m_font;
    Qt::Alignment m_alignment;
    bool m_ignoreAspectRatio;
    bool m_autoHeight;
    bool m_textDirection;
    bool m_textWrap;
    int m_radius;
    BarCode *m_barcode;
    Chart *m_chart;
    RptCrossTabObject *m_crossTab;

    QString m_formatString;
    QString m_highlighting;
    QString m_imgFormat;
    QPixmap m_pixmap;


};

#endif // GRAPHICSBOX_H

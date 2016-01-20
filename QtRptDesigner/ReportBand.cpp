/*
Name: QtRpt
Version: 1.5.5
Web-site: http://www.qtrpt.tk
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
Web-site: http://www.aliks-os.tk

Copyright 2012-2015 Aleksey Osipov

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

#include "ReportBand.h"
#include "ui_ReportBand.h"
#include <QObject>
#include "mainwindow.h"

ReportBand::ReportBand(QWidget *parent, BandType type, QTreeWidgetItem *item) :  QWidget(parent), ui(new Ui::ReportBand) {
    ui->setupUi(this);
    bandType = type;
    m_infocus = false;

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    //widget->releaseMouse();

    contWidget = ui->conWidget;
    itemInTree = item;

    ui->conWidget->setMouseTracking(true);
    ui->conWidget->installEventFilter(this);
    ui->lblBandType->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    titleHeight = 17;
    QObject::connect(ui->lblBandType, SIGNAL(clicked()), this, SLOT(setFocus()));
    mode = NONE;
    m_groupingField = "";
    m_startNewNumeration = false;
    m_showInGroup = false;
    m_startNewPage = false;
    QString stl = "QFrame#frame {background: silver;"
                  "border-top-left-radius: 10px;"
                  "border-top-right-radius: 10px;"
                  "border-right-width: 1px;"
                  "border-right-color: black;"
                  "border-right-style: solid;"
                  "border-top-width: 1px;"
                  "border-top-color: black;"
                  "border-top-style: solid;"
                  "border-left-width: 1px;"
                  "border-left-color: black;"
                  "border-left-style: solid; }";
    if (type == ReportTitle) {
        this->setObjectName("RepTitleBand");
        ui->lblBandType->setText(tr("Report title"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportTitle.png")));
    }
    if (type == ReportSummary) {
        this->setObjectName("ReportSummaryBand");
        ui->lblBandType->setText(tr("Report summary"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportFooter.png")));
    }
    if (type == PageHeader) {
        this->setObjectName("PageHeaderBand");
        ui->lblBandType->setText(tr("Page header"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageTitle.png")));
    }
    if (type == PageFooter) {
        this->setObjectName("PageFooterBand");
        ui->lblBandType->setText(tr("Page footer"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageFooter.png")));
    }
    if (type == MasterData) {
        this->setObjectName("MasterDataBand");
        ui->lblBandType->setText(tr("Master band"));
        stl = stl.replace("background: silver;","background: orange;");
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/data.png")));
    }
    if (type == MasterFooter) {
        this->setObjectName("MasterFooterBand");
        ui->lblBandType->setText(tr("Master footer"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataFooter.png")));
    }
    if (type == MasterHeader) {
        this->setObjectName("MasterHeaderBand");
        ui->lblBandType->setText(tr("Master header"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataFooter.png")));
    }
    if (type == DataGroupHeader) {
        this->setObjectName("DataGroupHeaderBand");
        ui->lblBandType->setText(tr("Data Group Header"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingHeader.png")));
    }
    if (type == DataGroupFooter) {
        this->setObjectName("DataGroupFooterBand");
        ui->lblBandType->setText(tr("Data Group Footer"));
        ui->frame->setStyleSheet(stl);
        ui->lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingFooter.png")));
    }
    ui->conWidget->setStyleSheet("#conWidget {border: 1px solid black}");
    setBaseSize(width(),height());
    scale = 1;
    newContLine = 0;  //!!!
    setTitleHeight(titleHeight);
    this->show();
}

void ReportBand::setTitleHeight(int h) {
    ui->lblBandType->setFixedHeight(h-1);
    ui->frame->setFixedHeight(h);
}

void ReportBand::setMenu(QMenu *menu_) {    
    QIcon icon;
    QAction *actBandDel = new QAction(tr("Delete"),this);
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/delete.png")), QIcon::Normal, QIcon::On);
    actBandDel->setObjectName("actBandDel");
    actBandDel->setIcon(icon);
    QObject::connect(actBandDel, SIGNAL(triggered()), this, SLOT(deleteLater()));

    this->actions().clear();
    this->addActions(menu_->actions());
    this->addAction(actBandDel);
}

void ReportBand::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Delete) {
        this->deleteLater();
    }
    if (QApplication::keyboardModifiers() == Qt::ShiftModifier) {
        if (e->key() == Qt::Key_Up) resize(width(),height()-1);
        if (e->key() == Qt::Key_Down) resize(width(),height()+1);
        setBaseSize(width()/scale,height()/scale);
    }
}

bool ReportBand::eventFilter( QObject *obj, QEvent *evt ) {
    //Draw Line of the TContainerLine
    if (obj == ui->conWidget) {        
        if (evt->type() == QEvent::Paint) {
            QList<TContainerLine *> allContList = this->contWidget->findChildren<TContainerLine *>();
            foreach(TContainerLine *cont, allContList) {
                if (!cont->line.p1().isNull()) {
                    QPainter painter(ui->conWidget);
                    painter.setRenderHint(QPainter::Antialiasing,true);
                    if (cont->line.p2().isNull()) {  //Drawing line, the end is Not set
                        painter.drawLine(cont->line.p1(), mousePos);
                    } else {  //Drawing all lines
                        if (cont->cs != 0 && cont->ce != 0 ) {
                            painter.setPen(QPen(cont->getColorValue(BorderColor),
                                                cont->getBorderWidth(),
                                                QtRPT::getPenStyle(cont->getBorderStyleStr()), Qt::RoundCap));
                            painter.drawLine(cont->cs->pos()+QPoint(3,3), cont->ce->pos()+QPoint(3,3));
                            cont->drawArrow(&painter);
                        }
                    }
                }
            }
        }
        if (evt->type() == QEvent::ChildAdded) {
            //after adding a child, Insert new item into the treeItem
            QChildEvent *e = static_cast< QChildEvent * >( evt );
            newFieldTreeItem(e->child());
        }
    }
    //!!!***************
    if ((evt->type() == QMouseEvent::MouseButtonPress) || (evt->type() == QMouseEvent::MouseMove)) {
        QMouseEvent *m = static_cast< QMouseEvent * >( evt );
        QWidget *w = QApplication::widgetAt( m->globalPos() );
        if( w ) {            
            if (m->type()==QMouseEvent::MouseButtonPress) { //Select TContainerLine by click
                mousePressEvent(m);
                if (this->contWidget) {
                    QList<TContainerLine *> allContList = this->contWidget->findChildren<TContainerLine *>();
                    for (int i=0; i<allContList.size(); i++) {
                        allContList.at(i)->setSelectedLine(m->pos());
                    }
                }

                if (this->newContLine) {
                    if (!this->newContLine->line.p1().isNull()) {  //SET the end of new line
                        this->newContLine->line.setP2(m->pos());
                        this->newContLine->ce->move(m->pos() - QPoint(3,3));
                        this->newContLine->ce->setVisible(true);
                        this->newContLine->setFocus();
                        this->newContLine->ce->setFocus();
                        this->newContLine->emitInFocus(true);

                        QList<RptContainer *> lst;
                        lst.append(this->newContLine);
                        getMW()->m_undoStack->push(new AddContainerCommand( lst ));
                        this->newContLine= 0;
                    }
                }
            }
            if (m->type()==QMouseEvent::MouseMove) {
                mouseMoveEvent(m);
                //!!!***************
                mousePos = QPoint(m->pos());
                if (this->newContLine)
                    ui->conWidget->repaint();
                //!!!***************
            }
        }
    }
    return QWidget::eventFilter(obj, evt);
}

void ReportBand::newFieldTreeItem(QObject *widget) {
    RptContainer *container = qobject_cast<RptContainer*>(widget);
    if (container != 0) {
        QIcon icon;
        itemInTree->treeWidget()->clearSelection();
        QTreeWidgetItem *item = new QTreeWidgetItem(itemInTree);
        container->itemInTree = item;
        if (container->getType() == Text || container->getType() == TextImage || container->getType() == DatabaseImage)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/field.png")), QIcon::Normal, QIcon::On);
        if (container->getType() == Image)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/picture.png")), QIcon::Normal, QIcon::On);
        if (container->getType() == Diagram)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/diagram.png")), QIcon::Normal, QIcon::On);
        if (QtRPT::getDrawingFields().contains(container->getType()))
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/drawing.png")), QIcon::Normal, QIcon::On);
        if (container->getType() == Barcode)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/barcode.png")), QIcon::Normal, QIcon::On);
        if (container->getType() == TextRich)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/richText.png")), QIcon::Normal, QIcon::On);
        if (container->getType() == CrossTab)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/crossTab.png")), QIcon::Normal, QIcon::On);
        item->setIcon(0,icon);
        item->setText(0,container->objectName());
        item->setSelected(true);
        itemInTree->addChild(item);
        itemInTree->setExpanded(true);
    }
}

void ReportBand::focusInEvent(QFocusEvent *e) {
    Q_UNUSED(e);
    m_infocus = true;
    ui->conWidget->setStyleSheet("#conWidget {border: 1px dot-dash blue}");
    emit inFocus(true);
}

void ReportBand::focusOutEvent(QFocusEvent *e) {
    Q_UNUSED(e);
    m_infocus = false;
    ui->conWidget->setStyleSheet("#conWidget {border: 1px solid black}");
    emit inFocus(false);    
}

void ReportBand::mouseReleaseEvent(QMouseEvent *e) {
    QWidget::mouseReleaseEvent(e);
}

void ReportBand::mousePressEvent(QMouseEvent *e) {
    position = QPoint(e->globalX()-geometry().x(), e->globalY()-geometry().y());
    QWidget::mouseMoveEvent(e);

    //if (!m_infocus) return;

    if (!(e->buttons() & Qt::LeftButton)) {
        setCursorShape(e->pos());
        return;
    }

    if(e->button()==Qt::RightButton) {
        TContainerLine *contLine = qobject_cast<TContainerLine*>(QApplication::focusWidget());
        if (contLine != 0) {
            QApplication::sendEvent(contLine, e);
            e->accept();
        }
    }
}

void ReportBand::setCursorShape(const QPoint &e_pos) {
    const int diff = 4;
    if ( e_pos.y() > y() + height() - diff) {
        setCursor(QCursor(Qt::SizeVerCursor));
        mode = RESIZEB;
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
        mode = NONE;
    }
}

void ReportBand::mouseMoveEvent(QMouseEvent *e) {
    QWidget::mouseMoveEvent(e);
    if (!m_infocus) return;

    if (/*mode = NONE &*/ !(e->buttons() & Qt::LeftButton)) {
        QPoint p = QPoint(e->x()+geometry().x(), e->y()+geometry().y());
        setCursorShape(p);
        return;
    }
    if ((mode == MOVE) && (e->buttons() & Qt::LeftButton)) {
        move(e->globalPos() - position);
    }
    if ((mode != MOVE) && (e->buttons() & Qt::LeftButton)) {
        switch (mode){
            case RESIZEB: {
                resize(width(),e->y());                
                break;}
            case RESIZER:  {
                resize(e->x(),height());
                break;}
            case RESIZEBR: {
                resize(e->x(),e->y());
                break;}
        }
        setBaseSize(width()/scale,height()/scale);
    }
}

void ReportBand::resizeEvent (QResizeEvent *event) {
    Q_UNUSED(event);
    endResizing(this->geometry());
}

QString ReportBand::getGroupingField() {
    return m_groupingField;
}

void ReportBand::setGroupingField(QString value) {
    m_groupingField = value;
}

int ReportBand::getStartNewNumertaion() {
    return m_startNewNumeration;
}

void ReportBand::setStartNewNumeration(bool value) {
    m_startNewNumeration = value;
}

bool ReportBand::getShowInGroup() {
    return m_showInGroup;
}

void ReportBand::setShowInGroup(bool value) {
    m_showInGroup = value;
}

bool ReportBand::getStartNewPage() {
    return m_startNewPage;
}

void ReportBand::setStartNewPage(bool value) {
    m_startNewPage = value;
}

ReportBand::~ReportBand() {
    emit delBand(this->itemInTree);
    delete ui;
}

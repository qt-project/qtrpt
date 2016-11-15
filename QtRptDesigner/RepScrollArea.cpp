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

#include "RepScrollArea.h"
#include "ui_RepScrollArea.h"
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QScrollBar>

RepScrollArea::RepScrollArea(QWidget *parent) : QScrollArea(parent), ui(new Ui::RepScrollArea) {
    ui->setupUi(this);
    m_mainWindow = parent;
    QScrollBar *bar = verticalScrollBar();
    QObject::connect(bar, SIGNAL(valueChanged(int)), this, SLOT(vScrolling(int)));

    scene = new GraphicsScene(this);
    scene->setSceneRect(0,0,800,800);
    QObject::connect(scene, SIGNAL(sceneClick()), m_mainWindow, SLOT(sceneClick()));
    QObject::connect(scene, SIGNAL(itemAdded(QGraphicsItem *)), m_mainWindow, SLOT(generateName(QGraphicsItem *)));
    QObject::connect(scene, SIGNAL(itemSelected(QGraphicsItem *)), m_mainWindow, SLOT(sceneItemSelectionChanged(QGraphicsItem *)));
    QObject::connect(scene, SIGNAL(itemDeleting(QGraphicsItem *, QTreeWidgetItem *)),
                     m_mainWindow, SLOT(delItemInTree(QGraphicsItem *, QTreeWidgetItem *)));
    ui->graphicsView->setContentsMargins(0,0,0,0);
    ui->graphicsView->setScene(scene);
    //ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);

    pageSetting.marginsLeft     = 40;
    pageSetting.marginsRight    = 40;
    pageSetting.marginsTop      = 40;
    pageSetting.marginsBottom   = 40;
    pageSetting.pageWidth       = 840;
    pageSetting.pageHeight      = 1188;
    pageSetting.pageOrientation = 0;
    pageSetting.border          = false;
    pageSetting.borderWidth     = 1;

    this->setMouseTracking(true);
    this->installEventFilter(parent);
    scene->installEventFilter(parent);
    
	ui->horRuler->installEventFilter(this);
    ui->verRuler->installEventFilter(this);

    this->setVisible(true);
    m_scale = 1;
    setPaperSize(0);

    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    ui->graphicsView->setGraphicsEffect(effect);
}

void RepScrollArea::vScrolling(int value) {
    Q_UNUSED(value);
    scene->update();
}

double RepScrollArea::setPaperSize(qreal scale) {
    if (scale == 0) {
        m_scale = 1;
    } else {
        if (qFabs(scale) > 1) { //Change zoom from combobox
            m_scale = scale/100;
            if (m_scale < 0.5) return -1;
        } else {                //Change zoom from wheel or click mouse
            if (scale>0) {
                m_scale+=0.25;
            } else {
                m_scale+=-0.25;
            }
        }

        if (m_scale < 0.5) { //Not allow zoom less than 50%
            m_scale = 0.5;
            return -1;
        }
    }

    scene->setMargins(pageSetting.marginsLeft,
                      pageSetting.marginsRight,
                      pageSetting.marginsTop,
                      pageSetting.marginsBottom);

    ui->graphicsView->setMinimumWidth(pageSetting.pageWidth*m_scale);
    ui->graphicsView->setMinimumHeight(pageSetting.pageHeight*m_scale);
    ui->leftMarginsSpacer->changeSize(pageSetting.marginsLeft*m_scale+26,
                                      ui->leftMarginsSpacer->sizeHint().height(),
                                      QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);
    ui->verticalLayout_10->invalidate();
    ui->topMarginsSpacer->changeSize(ui->topMarginsSpacer->sizeHint().width(),pageSetting.marginsTop*m_scale,QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->verticalLayout_52->invalidate();

    getKoef();

    return m_scale;
}

void RepScrollArea::setScale(const QString &scale) {
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() ;/// 100.0;
    setPaperSize(newScale);

    QMatrix oldMatrix = ui->graphicsView->matrix();
    ui->graphicsView->resetMatrix();
    ui->graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->graphicsView->scale(newScale/100, newScale/100);
}

qreal RepScrollArea::getScale() {
    return m_scale;
}

QList<QGraphicsItem *> RepScrollArea::getReportItems() {
    return scene->items();
}

void RepScrollArea::clearReport() {
    m_scale = 1;
    setPaperSize(100);
    scene->clear();
    scene->update();
}

bool RepScrollArea::allowField() {
    bool hasBand = false;
    for(auto item : scene->items()){
        if (item->type() == ItemType::GBand) hasBand = true;
    }
    return hasBand;
}

void RepScrollArea::showGrid(bool value) {
    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("ShowGrid", value);
    this->isShowGrid = value;
    scene->isShowGrid = value;
    scene->setGridStep(settings.value("GridStep",1).toDouble());
    scene->setMargins(pageSetting.marginsLeft,
                      pageSetting.marginsRight,
                      pageSetting.marginsTop,
                      pageSetting.marginsBottom);
    getKoef();
    scene->setMesKoef(koef);
}

void RepScrollArea::getKoef() {
    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");

    settings.beginGroup("language");
    QString measurement = settings.value("measurement").toString();
    settings.endGroup();

    if (measurement == "")
        koef = 40;
    else if (measurement == "Cm")
        koef = 40;
    else if (measurement == "Inch")
        koef = 101.59;
    koef = koef*m_scale;
}

void RepScrollArea::paintHorRuler() {
    getKoef();
    QPainter painter(ui->horRuler);
    double x_=0 ;
    bool showNum = false;
    while ( x_ < ui->horRuler->width() ) {
        x_ = x_+koef/2;
        if (showNum) {
            if (this->isLeftToRight()) {
                const QString rt = QString::number(x_/koef);
                painter.drawText(x_,15,rt);
            }
            if (this->isRightToLeft()) {
                const QString rt = QString::number(x_/koef);
                painter.drawText(ui->horRuler->width() - x_,15,rt);
            }
        } else {
            if (m_scale > 0.5) {
                if (this->isLeftToRight()) {
                    painter.drawText(x_,15,"-");
                }
                if (this->isRightToLeft()) {
                    painter.drawText(ui->horRuler->width() - x_,15,"-");
                }
            }
        }
        showNum=!showNum;
    }
}

void RepScrollArea::paintVerRuler() {
    getKoef();
    QPainter painter(ui->verRuler);
    double y_ =0 ;
    painter.rotate(-90);
    bool showNum = false;
    while ( y_ < ui->verRuler->height() ) {
        y_ = y_+koef/2;
        if (showNum) {
            const QString rt = QString::number(y_/koef);
            painter.drawText(-y_,15,rt);
        } else {
            if (m_scale > 0.5)
                painter.drawText(-y_,15,"-");
        }
        showNum=!showNum;
    }
}

ReportBand *RepScrollArea::m_addBand(BandType type, QMenu *bandMenu, int m_height) {
    QMenu m_bandMenu;
    for(auto action : bandMenu->actions()) {
        if (type == DataGroupHeader) {
            m_bandMenu.addAction(action);
        } else {
            if (action->objectName() != "actGroupProperty") {
                m_bandMenu.addAction(action);
            }
        }
    }

    ReportBand *reportBand = new ReportBand(type);
    reportBand->setMenu(&m_bandMenu);
    reportBand->setZValue(10);

    reportBand->setWidth(pageSetting.pageWidth - pageSetting.marginsLeft - pageSetting.marginsRight);
    if (m_height != 0)
        reportBand->setHeight(m_height);
    else
        reportBand->setHeight(200);
    scene->addItem(reportBand);
    this->newFieldTreeItem(reportBand);
    reportBand->setSelected(true);

    QPointF p = ui->graphicsView->mapToScene(0,0);
    p.setX( p.x() + pageSetting.marginsLeft  ) ;
    p.setY( p.y() + pageSetting.marginsTop  ) ;
    reportBand->setPos(p);

    correctBandGeom();

    QObject::connect(reportBand, SIGNAL(itemRemoving()), scene, SLOT(itemRemoving()));
    return reportBand;
}

void RepScrollArea::newFieldTreeItem(QGraphicsItem *item) {
    ReportBand *gBand = nullptr;
    GraphicsBox *gItem = nullptr;
    GraphicsLine *gLine = nullptr;

    if (item->type() == ItemType::GBand)
        gBand = static_cast<ReportBand *>(item);
    if (item->type() == ItemType::GBox)
        gItem = static_cast<GraphicsBox *>(item);
    if (item->type() == ItemType::GLine)
        gLine = static_cast<GraphicsLine *>(item);

    QIcon icon;
    if (gBand != nullptr) {
        rootItem->treeWidget()->clearSelection();

        QTreeWidgetItem *t_item = new QTreeWidgetItem(rootItem);
        icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/bands.png")), QIcon::Normal, QIcon::On);
        t_item->setIcon(0,icon);
        gBand->itemInTree = t_item;

        t_item->setText(0,gBand->objectName());
        t_item->setSelected(true);
        rootItem->setExpanded(true);

        for (int i=0; i<item->childItems().size(); i++)
            newFieldTreeItem(gBand->childItems().at(i));
    }
    if (gItem != nullptr) {
        QTreeWidgetItem *bandItem = static_cast<GraphicsBox*>(gItem->parentItem())->itemInTree;

        rootItem->treeWidget()->clearSelection();
        QTreeWidgetItem *item = new QTreeWidgetItem(bandItem);
        gItem->itemInTree = item;
        if (gItem->getFieldType() == Text || gItem->getFieldType() == TextImage || gItem->getFieldType() == DatabaseImage)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/field.png")), QIcon::Normal, QIcon::On);
        if (gItem->getFieldType() == Image)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/picture.png")), QIcon::Normal, QIcon::On);
        if (gItem->getFieldType() == Diagram)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/diagram.png")), QIcon::Normal, QIcon::On);
        if (QtRPT::getDrawingFields().contains(gItem->getFieldType()))
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/drawing.png")), QIcon::Normal, QIcon::On);
        if (gItem->getFieldType() == Barcode)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/barcode.png")), QIcon::Normal, QIcon::On);
        if (gItem->getFieldType() == TextRich)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/richText.png")), QIcon::Normal, QIcon::On);
        if (gItem->getFieldType() == CrossTab)
            icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/crossTab.png")), QIcon::Normal, QIcon::On);
        item->setIcon(0,icon);
        item->setText(0,gItem->objectName());
        item->setSelected(true);
        rootItem->addChild(item);
        bandItem->setExpanded(true);
    }
    if (gLine != nullptr) {
        QTreeWidgetItem *bandItem = static_cast<GraphicsBox*>(gLine->parentItem())->itemInTree;

        rootItem->treeWidget()->clearSelection();
        QTreeWidgetItem *item = new QTreeWidgetItem(bandItem);
        gLine->itemInTree = item;

        icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/line2.png")), QIcon::Normal, QIcon::On);
        item->setIcon(0,icon);
        item->setText(0,gLine->objectName());
        item->setSelected(true);
        rootItem->addChild(item);
        bandItem->setExpanded(true);
    }
}

//Correct band position after inserting, deleteing
void RepScrollArea::correctBandGeom(ReportBand *rep) {
    QPointF p = ui->graphicsView->mapToScene(0,0);
    int top_ = p.y()+pageSetting.marginsTop;

    QList<ReportBand *> allReportBand = getReportBands();
    if (allReportBand.size() != 0)
        qSort(allReportBand.begin(), allReportBand.end(), compareBandType);

    for(auto band : allReportBand) {
        if (band == rep) continue;
        band->setPos( QPointF(band->pos().x(), top_) );
        top_ += band->getHeight()+15;
    }
}

bool RepScrollArea::eventFilter(QObject *obj, QEvent *e) {
    if(obj==ui->horRuler && e->type()==QEvent::Paint) {
        paintHorRuler();
        return true;
    }
    if(obj==ui->verRuler && e->type()==QEvent::Paint) {
        paintVerRuler();        
        return true;
    }
    return QWidget::eventFilter(obj,e);
}

QList<ReportBand *> RepScrollArea::getReportBands() {
    QList<ReportBand *> allReportBand;
    for(auto item : scene->items())
        if (item->type() == ItemType::GBand)
            allReportBand << static_cast<ReportBand*>(item);
    return allReportBand;
}

RepScrollArea::~RepScrollArea() {
    delete ui;
}

int compareBandType(ReportBand *p1, ReportBand *p2) {
    return p1->bandType < p2->bandType;
}

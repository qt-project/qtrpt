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

#include "RepScrollArea.h"
#include "ui_RepScrollArea.h"
#include <QGraphicsDropShadowEffect>
#include <QSettings>

RepScrollArea::RepScrollArea(QWidget *parent) : QScrollArea(parent), ui(new Ui::RepScrollArea) {
    ui->setupUi(this);
    m_mainWindow = parent;
    ui->repWidget->setObjectName("repWidget");
    repWidget = ui->repWidget;

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
    ui->repWidget->setMouseTracking(true);
    ui->repWidget->installEventFilter(parent);
    //ui->repWidget->installEventFilter(this);
    
	ui->horRuler->installEventFilter(this);
    ui->verRuler->installEventFilter(this);

    overlay = new Overlay(ui->repWidget);
    overlay->resize(ui->repWidget->size());
    overlay->setVisible(true);

    this->setVisible(true);
    m_scale = 1;
    setPaperSize(0);

    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(5);
    ui->repWidget->setGraphicsEffect(effect);
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

    setUpdatesEnabled(false);
    ui->repWidget->setMinimumWidth(pageSetting.pageWidth*m_scale);
    ui->repWidget->setMinimumHeight(pageSetting.pageHeight*m_scale);
    ui->leftMarginsSpacer->changeSize(pageSetting.marginsLeft*m_scale+26,
                                      ui->leftMarginsSpacer->sizeHint().height(),
                                      QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);
    ui->verticalLayout_10->invalidate();
    ui->topMarginsSpacer->changeSize(ui->topMarginsSpacer->sizeHint().width(),pageSetting.marginsTop*m_scale,QSizePolicy::Fixed,QSizePolicy::Fixed);
    ui->verticalLayout_52->invalidate();

    QList<ReportBand *> allReportBand = ui->repWidget->findChildren<ReportBand *>();
    qSort(allReportBand.begin(), allReportBand.end(), compareBandType);
    int top_ = pageSetting.marginsTop*m_scale;
    foreach(ReportBand *band, allReportBand) {
        band->scale = m_scale;
        band->setTitleHeight(band->titleHeight*m_scale);
        band->setGeometry(pageSetting.marginsLeft*m_scale,
                          top_,
                          (pageSetting.pageWidth*m_scale - pageSetting.marginsLeft*m_scale - pageSetting.marginsRight*m_scale ),
                          band->baseSize().height()*m_scale);
        top_ += band->geometry().height() + 5*m_scale;  //Step between bands

        foreach(TContainerField *cont, band->findChildren<TContainerField *>()) {
            qreal x_ = qRound(cont->x()/cont->scale);
            qreal y_ = qRound(cont->y()/cont->scale);

            cont->scale = m_scale;
            cont->setGeometry(qRound(x_*m_scale),
                              qRound(y_*m_scale),
                              qRound(cont->baseSize().width()*m_scale),
                              qRound(cont->baseSize().height()*m_scale));
            cont->setFontSize(cont->getFontSize());
        }
    }
    overlay->resize(ui->repWidget->size());
    setUpdatesEnabled(true);
    getKoef();
    return m_scale;
}

qreal RepScrollArea::getScale() {
    return m_scale;
}

QWidgetList RepScrollArea::getReportItems() {
    return ui->repWidget->findChildren<QWidget *>();
}

void RepScrollArea::clearReport() {
    m_scale = 1;
    qDeleteAll(ui->repWidget->findChildren<ReportBand*>());
    setPaperSize(100);
}

bool RepScrollArea::allowField() {
    QList<ReportBand *> allReportBand = ui->repWidget->findChildren<ReportBand *>();
    if (allReportBand.size() == 0) return false;
    else return true;
}

void RepScrollArea::paintGrid() {
    if(!isShowGrid) return;
    QPainter painter(ui->repWidget);
    if (!painter.isActive()) return;

    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    getKoef();

    int x_=pageSetting.marginsLeft*m_scale+koef * settings.value("GridStep",1).toDouble();
    int y_=pageSetting.marginsTop*m_scale;

    painter.setPen(QColor(240, 240, 240, 255));
    painter.drawRect(pageSetting.marginsLeft*m_scale,pageSetting.marginsTop*m_scale,
                     ui->repWidget->width()-pageSetting.marginsRight*m_scale-pageSetting.marginsLeft*m_scale,
                     ui->repWidget->height()-pageSetting.marginsTop*m_scale-pageSetting.marginsBottom*m_scale);

    while ( x_ < ui->repWidget->width()-pageSetting.marginsRight*m_scale ) {
        painter.drawLine(x_,pageSetting.marginsTop*m_scale,x_,ui->repWidget->height()-pageSetting.marginsTop*m_scale);
        x_ = x_+koef * settings.value("GridStep",1).toDouble();
    }

    while ( y_ < ui->repWidget->height()-pageSetting.marginsTop*m_scale ) {
        painter.setOpacity(0.5);
        painter.drawLine(pageSetting.marginsLeft*m_scale,y_,ui->repWidget->width()-pageSetting.marginsRight*m_scale,y_);
        y_ = y_+koef * settings.value("GridStep",1).toDouble();
    }
}

void RepScrollArea::showGrid(bool value) {
    QSettings settings(QCoreApplication::applicationDirPath()+"/setting.ini",QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("ShowGrid", value);
    this->isShowGrid = value;

    getKoef();
    ui->repWidget->repaint();
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

ReportBand *RepScrollArea::m_addBand(QString bandName, BandType type, QMenu *bandMenu, int m_height) {
    QMenu m_bandMenu;
    foreach(QAction *action, bandMenu->actions()) {
        if (type == DataGroupHeader) {
            m_bandMenu.addAction(action);
        } else {
            if (action->objectName() != "actGroupProperty") {
                m_bandMenu.addAction(action);
            }
        }
    }

    QTreeWidgetItem *item = new QTreeWidgetItem(rootItem);
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/bands.png")), QIcon::Normal, QIcon::On);
    item->setIcon(0,icon);
    item->setText(0,bandName);
    item->setSelected(true);
    rootItem->addChild(item);

    ReportBand *reportBand = new ReportBand(ui->repWidget, type, item);
    reportBand->setMenu(&m_bandMenu);
    reportBand->itemInTree = item;
    QObject::connect(reportBand, SIGNAL(delBand(QTreeWidgetItem *)), m_mainWindow, SLOT(delItemInTree(QTreeWidgetItem *)));

    QRect r = reportBand->geometry();
    if (m_height != 0) {
        r.setHeight(m_height*m_scale+reportBand->titleHeight);
    } else {
        r.setHeight(reportBand->baseSize().height()*m_scale);
    }
    r.setLeft(pageSetting.marginsLeft*m_scale);
    r.setWidth(pageSetting.pageWidth*m_scale - pageSetting.marginsLeft*m_scale - pageSetting.marginsRight*m_scale);
    reportBand->setGeometry(r);
    reportBand->setBaseSize(r.width()/m_scale,r.height()/m_scale);
    reportBand->scale = m_scale;

    correctBandGeom();

    QObject::connect(reportBand, SIGNAL(endResizing(QRect)), this, SLOT(bandResing(QRect)));
    QObject::connect(reportBand, SIGNAL(inFocus(bool)), m_mainWindow, SLOT(setWidgetInFocus(bool)));
    return reportBand;
}

//Correct band position after inserting, deleteing
void RepScrollArea::correctBandGeom(QWidget *rep) {
    int top_ = pageSetting.marginsTop*m_scale;
    QList<ReportBand *> allReportBand = ui->repWidget->findChildren<ReportBand *>();
    if (allReportBand.size() != 0)
        qSort(allReportBand.begin(), allReportBand.end(), compareBandType);

    foreach(ReportBand *band, allReportBand) {
        if (band == rep) continue;
        band->move(pageSetting.marginsLeft*m_scale, top_);
        top_ += band->height()+5;
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

void RepScrollArea::bandResing(QRect rect) {
    ReportBand *reportBand = qobject_cast<ReportBand *>(sender());
    QList<ReportBand *> allReportBand = ui->repWidget->findChildren<ReportBand *>();
    qSort(allReportBand.begin(), allReportBand.end(), compareBandType);
    int top_ = rect.y()+rect.height()+5;

    foreach(ReportBand *band, allReportBand) {
        if (band->bandType <= reportBand->bandType) continue;
        band->setGeometry(pageSetting.marginsLeft*m_scale,
                            top_,
                            (pageSetting.pageWidth*m_scale - pageSetting.marginsLeft*m_scale - pageSetting.marginsRight*m_scale ),
                            band->baseSize().height()*m_scale);
        top_ += band->geometry().height()+5;
    }

    int _h = reportBand->geometry().height() - reportBand->titleHeight;
    emit bandResing(_h);
}

RepScrollArea::~RepScrollArea() {
    delete ui;
}

int compareBandType(ReportBand *p1, ReportBand *p2) {
    return p1->bandType < p2->bandType;
}

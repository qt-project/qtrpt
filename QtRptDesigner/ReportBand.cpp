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

#include "ReportBand.h"
#include "ui_ReportBand.h"
#include <QObject>
#include "mainwindow.h"

ReportBand::ReportBand(BandType type) {
    setFlag(QGraphicsItem::ItemIsMovable,false);
    m_menu = new QMenu;
    bandType = type;
    m_infocus = false;
    itemInTree = 0;
    titleHeight = 20;

    m_groupingField = "";
    m_startNewNumeration = false;
    m_showInGroup = false;
    m_startNewPage = false;

    if (type == ReportTitle) {
        this->setObjectName("RepTitleBand");
        setText(QObject::tr("Report title"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportTitle.png")));
    }
    if (type == ReportSummary) {
        this->setObjectName("ReportSummaryBand");
        setText(QObject::tr("Report summary"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/reportFooter.png")));
    }
    if (type == PageHeader) {
        this->setObjectName("PageHeaderBand");
        setText(QObject::tr("Page header"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageTitle.png")));
    }
    if (type == PageFooter) {
        this->setObjectName("PageFooterBand");
        setText(QObject::tr("Page footer"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/pageFooter.png")));
    }
    if (type == MasterData) {
        this->setObjectName("MasterDataBand");
        setText(QObject::tr("Master band"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/data.png")));
    }
    if (type == MasterFooter) {
        this->setObjectName("MasterFooterBand");
        setText(QObject::tr("Master footer"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataFooter.png")));
    }
    if (type == MasterHeader) {
        this->setObjectName("MasterHeaderBand");
        setText(QObject::tr("Master header"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataFooter.png")));
    }
    if (type == DataGroupHeader) {
        this->setObjectName("DataGroupHeaderBand");
        setText(QObject::tr("Data Group Header"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingHeader.png")));
    }
    if (type == DataGroupFooter) {
        this->setObjectName("DataGroupFooterBand");
        setText(QObject::tr("Data Group Footer"));
        setBandPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/dataGroupingFooter.png")));
    }   
}

void ReportBand::setMenu(QMenu *menu_) {
    QIcon icon;
    QAction *actBandDel = new QAction(tr("Delete"),this);
    icon.addPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/images/delete.png")), QIcon::Normal, QIcon::On);
    actBandDel->setObjectName("actBandDel");
    actBandDel->setIcon(icon);
    QObject::connect(actBandDel, SIGNAL(triggered()), this, SIGNAL(itemRemoving()));

    m_menu->actions().clear();
    m_menu->addActions(menu_->actions());
    m_menu->addAction(actBandDel);
}

void ReportBand::setHeight(qreal value) {
    GraphicsBox::setHeight(titleHeight + value);
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

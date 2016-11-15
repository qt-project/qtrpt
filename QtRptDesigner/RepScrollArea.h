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

#ifndef REPSCROLLAREA_H
#define REPSCROLLAREA_H

#include <QScrollArea>
#include "PageSettingDlg.h"
#include "ReportBand.h"
#include "GraphicsScene.h"

namespace Ui {
    class RepScrollArea;
}

int compareBandType(ReportBand *p1, ReportBand *p2);

class RepScrollArea : public QScrollArea
{
    Q_OBJECT
    
public:
    explicit RepScrollArea(QWidget *parent = 0);
    QWidget *repWidget;
    ~RepScrollArea();
    double setPaperSize(qreal scale);
    bool isShowGrid;
    ReportBand *m_addBand(BandType type, QMenu *bandMenu, int m_height=0);
    void newFieldTreeItem(QGraphicsItem *item);
    PageSetting pageSetting;
    void correctBandGeom(ReportBand *rep = 0);
    bool allowField();
    void clearReport();
    QList<QGraphicsItem *> getReportItems();
    QTreeWidgetItem *rootItem;
    QIcon icon;
    qreal getScale();
    GraphicsScene * scene;
    void setScale(const QString &scale);
    QList<ReportBand *> getReportBands();

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::RepScrollArea *ui;
    void paintHorRuler();
    void paintVerRuler();
    QWidget *m_mainWindow;
    double koef;
    qreal m_scale;
    void getKoef();

public slots:
    void showGrid(bool value);

private slots:
    void bandResing(QRect rect);
    void vScrolling(int value);

signals:
    void bandResing(int);

};

#endif // REPSCROLLAREA_H

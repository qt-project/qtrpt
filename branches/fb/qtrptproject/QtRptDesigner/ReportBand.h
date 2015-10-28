/*
Name: QtRpt
Version: 1.5.4
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

#ifndef REPORTBAND_H
#define REPORTBAND_H

#include <QWidget>
#include <QObject>
#include <QMouseEvent>
#include <QTreeWidgetItem>
#include <xyz_Label.h>
#include "TContainerField.h"
#include "TContainerLine.h"

namespace Ui {
    class ReportBand;
}

class ReportBand : public QWidget
{
    Q_OBJECT
    
public:
    BandType bandType;
    int titleHeight;
    QTreeWidgetItem *itemInTree;
    explicit ReportBand(QWidget *parent = 0, BandType type = ReportTitle, QTreeWidgetItem *item = 0);

    QWidget *contWidget;
    TContainerLine *newContLine;
    void newFieldTreeItem(QObject *widget);
    void setMenu(QMenu *menu_);
    qreal scale;
    void setTitleHeight(int h);
    QString getGroupingField();
    void setGroupingField(QString value);
    int getStartNewNumertaion();
    void setStartNewNumeration(bool value);
    bool getShowInGroup();
    void setShowInGroup(bool value);
    bool getStartNewPage();
    void setStartNewPage(bool value);
    ~ReportBand();

private:
    int mode;
    bool m_infocus;
    void setCursorShape(const QPoint &e_pos);
    QString m_groupingField;
    bool m_startNewNumeration;
    bool m_showInGroup;
    bool m_startNewPage;
    Ui::ReportBand *ui;

signals:
    void inFocus(bool mode);
    void endResizing(QRect rect);
    void delBand(QTreeWidgetItem *);

protected:
    QPoint position;
    QPointF mousePos;
    bool eventFilter( QObject *obj, QEvent *evt );
    void resizeEvent (QResizeEvent *event);
    void focusInEvent(QFocusEvent *);
    void focusOutEvent(QFocusEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
};

#endif // REPORTBAND_H

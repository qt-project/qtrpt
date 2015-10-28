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

#ifndef SQLDESIGNER_H
#define SQLDESIGNER_H

#include <QWidget>
#include <QDomDocument>
#include <QtSql>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QGraphicsScene>
#include "diagramdocument.h"

namespace Ui {
    class SqlDesigner;
}

class SqlDesigner : public QWidget
{
    Q_OBJECT

public:
    explicit SqlDesigner(QWidget *parent = 0);
    void showDataSource(QDomElement e);
    QDomElement saveParamToXML(QDomDocument *xmlDoc);
    ~SqlDesigner();

protected:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::SqlDesigner *ui;
    QSqlDatabase db;
    DiagramDocument *scene = nullptr;
    void refreshTable(QSqlDatabase *db);
    void newDiagramDocument();

private slots:
    void rbChecked();
    void connectDB();
    void btnClose();
    void addRelation();
    void clearDiagram();
    void undo();
    void redo();
    void select();
    void updateMode(DiagramDocument::Mode mode);
    void deleteSelected();
    void sqlChanged(const QString value);
    void selectXMLFile();
    void previewXMLData();

signals:
    void changed(bool value);
};

#endif // SQLDESIGNER_H

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

#ifndef RPTCROSSTABOBJECT_H
#define RPTCROSSTABOBJECT_H

#include <QDebug>
#include <QMetaType>
#include <RptFieldObject.h>

using namespace QtRptName;

class RptFieldObject;

struct RptTabElement {
    RptFieldObject *fieldObject;
    QVariant value;
    int left;
    int top;
    int height;
    int width;
    int corrLeft;
    int corrTop;
};

typedef QVector<RptTabElement> VectorRptTabElement;


class RptCrossTabObject
{
public:
    RptCrossTabObject();
    ~RptCrossTabObject();
    QString name;
    QRect rect;
    bool isColHeaderVisible() const {return colHeaderVisible;}
    bool isRowHeaderVisible() const {return rowHeaderVisible;}
    void setColHeaderVisible(bool value) {colHeaderVisible = value;}
    void setRowHeaderVisible(bool value) {rowHeaderVisible = value;}
    void addCol(QString colName);
    void addRow(QString rowName);
    QString getColName(int col) const;
    QString getRowName(int row) const;
    int colCount() const;
    int rowCount() const;

    void initMatrix();
    QVariant getMatrixValue(int col,int row) const;
    void setMatrixValue(int col,int row, QVariant value);
    QVector<VectorRptTabElement > valuesArray;

    void makeFeelMatrix();
    QList<RptFieldObject*> fieldList;
    RptFieldObject *parentField;

    void addElement(RptTabElement element);
    void appendRow();
    void appendColumn();

private:
    bool colHeaderVisible;
    bool rowHeaderVisible;
    QStringList m_colHeader;
    QStringList m_rowHeader;
    int m_colCount;
    int m_rowCount;

    void addField(RptFieldObject *field);

    QVector<int> colVector;
    QVector<int> rowVector;

};

Q_DECLARE_METATYPE(RptCrossTabObject)
QDebug operator<<(QDebug dbg, const RptCrossTabObject &obj);
QDebug operator<<(QDebug dbg, const RptCrossTabObject *obj);

#endif // RPTCROSSTABOBJECT_H

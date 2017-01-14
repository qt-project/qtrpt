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
    int colCount() const;
    void setColCount(int value);
    int rowCount() const;
    void setRowCount(int value);
    int rowHeight();
    void setRowHeight(int height);
    //****
    bool isColHeaderVisible() const {return colHeaderVisible;}
    bool isRowHeaderVisible() const {return rowHeaderVisible;}
    void setColHeaderVisible(bool value) {colHeaderVisible = value;}
    void setRowHeaderVisible(bool value) {rowHeaderVisible = value;}

    void clear();
    QVector<VectorRptTabElement > valuesArray;

    void makeFeelMatrix();
    QList<RptFieldObject*> fieldList;
    RptFieldObject *parentField;

    void addElement(RptTabElement element);
    void resortMatrix();

private:
    int m_colCount;
    int m_rowCount;
    int m_rowHeight;
    //****
    bool colHeaderVisible;
    bool rowHeaderVisible;
    bool colTotalExists;
    bool rowTotalExists;
    QStringList m_colHeader;

	QString	stTotal;

    void addField(RptFieldObject *field);

    QVector<int> colVector;
    QVector<int> rowVector;

};

Q_DECLARE_METATYPE(RptCrossTabObject)
QDebug operator<<(QDebug dbg, const RptCrossTabObject &obj);
QDebug operator<<(QDebug dbg, const RptCrossTabObject *obj);

#endif // RPTCROSSTABOBJECT_H

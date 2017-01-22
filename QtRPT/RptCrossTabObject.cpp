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

#include "RptCrossTabObject.h"

RptCrossTabObject::RptCrossTabObject() {
    name = "RptCrossTabObject_DEMO";
    rect.setX(0);
    rect.setY(0);
    rect.setWidth(500);
    rect.setHeight(500);
	stTotal = QObject::tr("Total");
    m_rowHeight = 20;
    m_rowCount = 3;
    m_colCount = 3;

    qRegisterMetaType<RptCrossTabObject>( "RptCrossTabObject" );
}

int RptCrossTabObject::rowHeight() {
    return m_rowHeight;
}

void RptCrossTabObject::setRowHeight(int height) {
    m_rowHeight = height;
}

int RptCrossTabObject::colCount() const {
    return m_colCount;
}

void RptCrossTabObject::setColCount(int value) {
    m_colCount = value;
}

int RptCrossTabObject::rowCount() const {
    return m_rowCount;
}

void RptCrossTabObject::setRowCount(int value) {
    m_rowCount = value;
}

void RptCrossTabObject::buildMatrix() {
    float fieldWidth = rect.width();
    float fieldheight = rect.height();
    if (m_colCount == 0) return;
    if (m_rowCount == 0) return;

    fieldWidth = rect.width()/colCount();
    fieldheight = rowHeight();

    for (int row=0; row < m_rowCount; row++) {
        for (int col=0; col < m_colCount; col++) {
            auto h1 = new RptFieldObject();
            h1->parentCrossTab = this;
            h1->name = QString("f%1%2").arg(col).arg(row);
            h1->fieldType = Text;
            h1->rect.setTop(rect.top() + fieldheight*row);
            h1->rect.setLeft(rect.left() + fieldWidth*col);
            h1->rect.setHeight(fieldheight);
            h1->rect.setWidth(fieldWidth);
            //h1->value =  QString("f%1%2").arg(col).arg(row);  //getMatrixValue(col,row).toString();
            h1->aligment = Qt::AlignCenter;
            addField(h1);  //Append field
        }
    }
}

/*!
 \fn int RptCrossTabObject::fieldRow(RptFieldObject* field)
    Define the row of the \a field RptCrossTabObject object.

    \sa fieldCol
*/
int RptCrossTabObject::fieldRow(RptFieldObject* field) {
    int index = fieldList.indexOf(field);
    if (index != -1) {
        unsigned row = unsigned(index / m_colCount);
        //unsigned column = index - row * m_colCount;
        index = row;
    }
    return index;
}

/*!
 \fn int RptCrossTabObject::fieldCol(RptFieldObject* field)
    Define the column of the \a field RptCrossTabObject object.

    \sa fieldRow
*/
int RptCrossTabObject::fieldCol(RptFieldObject* field) {
    int index = fieldList.indexOf(field);
    if (index != -1) {
        unsigned row = unsigned(index / m_colCount);
        unsigned column = index - row * m_colCount;
        index = column;
    }
    return index;
}

/*!
 \fn void RptCrossTabObject::addField(RptFieldObject *field)
    Add \a field to current RptCrossTabObject object.

    \sa RptFieldObject
*/
void RptCrossTabObject::addField(RptFieldObject *field) {
    fieldList.append(field);
}

/*!
  Destroys the object, deleting all its child objects.
 */
RptCrossTabObject::~RptCrossTabObject() {
    for (int i=0; i<fieldList.size(); i++)
        if (fieldList.at(i) != 0)
            delete fieldList.at(i);
    fieldList.clear();
}

QDebug operator<<(QDebug dbg, const RptCrossTabObject &obj) {
    dbg << obj.name << "\n";
    return dbg;
}

QDebug operator<<(QDebug dbg, const RptCrossTabObject *obj) {
    return dbg << *obj;
}

//Bellow functions for working with a grid
/*!
 \fn void RptCrossTabObject::addElement(RptTabElement element)
    Add \a element as a RptFieldObject to the matrix.
    This function is only required when RptCrossTabObject is used when
    carry out report export to Excel.
*/
void RptCrossTabObject::addElement(RptTabElement element) {
//    int correlation = 50;
//    int tmpCol = 0, tmpRow = 0;

//    //---
//    bool fnd = false;
//    for (int col=0; col < colVector.size(); col++) {
//        if (element.left <= colVector.at(col)+correlation &&
//            element.left >= colVector.at(col)-correlation ) {
//            fnd = true;
//            element.corrLeft = colVector.at(col);
//            tmpCol = col;
//            break;
//        }
//    }
//    if (!fnd) {
//        colVector.append(element.left);
//        tmpCol = appendColumn(QString("%1").arg(element.left));
//    }
//    //---
//    fnd = false;
//    for (int row=0; row < rowVector.size(); row++) {
//        if (element.top <= rowVector.at(row)+correlation &&
//            element.top >= rowVector.at(row)-correlation ) {
//            fnd = true;
//            //qDebug()<< element.top << rowVector.at(row)+correlation << rowVector.at(row)-correlation;
//            element.corrTop = rowVector.at(row);
//            tmpRow = row;
//            break;
//        }
//    }
//    if (!fnd) {
//        rowVector.append(element.top);
//        tmpRow = appendRow(QString("%1").arg(element.top));
//    }
//    initMatrix();
//    setMatrixElement(tmpCol,tmpRow,element);
}

/*!
 \fn void RptCrossTabObject::resortMatrix()
    This function resort Matrix which stores values of RptTabElement.
    This function is only required when RptCrossTabObject is used when
    carry out report export to Excel. In this case matrix hold on values
    of all fields. m_colHeader and m_rowHeader lists hold a numbers of positions
    of the each field, to right export to Excel grid, these numbers should be resorted
    and transfer to row(col) number of Excel.

    \sa RptTabElement element
*/
void RptCrossTabObject::resortMatrix() {
    //resort rows
//    int n = m_rowHeader.size();
//    for (int i=0; i<n; ++i)
//        for(int j=i+1; j<n; ++j)
//            if (QString(m_rowHeader[j]).toInt() < QString(m_rowHeader[i]).toInt()) {
//                qSwap(m_rowHeader[i], m_rowHeader[j]);
//                qSwap(valuesArray[i], valuesArray[j]);
//            }

//    //resort columns
//    n = m_colHeader.size();
//    for(int i=0; i<n; ++i)
//        for(int j=i+1; j<n; ++j)
//            if(QString(m_colHeader[j]).toInt() < QString(m_colHeader[i]).toInt()) {
//                qSwap(m_colHeader[i], m_colHeader[j]);
//                for (int row=0; row<m_rowHeader.size(); row++) {
//                    qSwap(valuesArray[row][i], valuesArray[row][j]);
//                }
//            }
}


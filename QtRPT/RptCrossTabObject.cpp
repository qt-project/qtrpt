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
    colHeaderVisible = false;
    rowHeaderVisible = false;
    colTotalVisible = false;
    rowTotalVisible = false;
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

// Set vibility to Column Total, set colTotalExists
void RptCrossTabObject::setColTotalVisible(bool value) {
    colTotalVisible = value;
}

// Set vibility to Row Total, set rowTotalExists
void RptCrossTabObject::setRowTotalVisible(bool value) {
    rowTotalVisible = value;
}

void RptCrossTabObject::clear() {
    colVector.clear();
    rowVector.clear();
    m_colHeader.clear();
    valuesArray.clear();
}

void RptCrossTabObject::makeFeelMatrix() {
//    qDebug() << "makeFeelMatrix" << "Col:" << colCount() << "Row:" << rowCount();
//    float fieldWidth = rect.width();
//    float fieldheight = rect.height();
//    if (colCount() == 0) return;
//    if (rowCount() == 0) return;

//    fieldWidth = rect.width()/colCount();
//    fieldheight = rect.height()/rowCount();

//    if (isRowHeaderVisible() && isColHeaderVisible()) {
//        RptFieldObject *h1 = new RptFieldObject();
//        h1->name = QString("TopLeftCorner");
//        h1->fieldType = Text;
//        h1->rect.setTop(rect.top() );
//        h1->rect.setLeft(rect.left());
//        h1->rect.setHeight(fieldheight);
//        h1->rect.setWidth(fieldWidth);
//        h1->value = QString("");
//        h1->font.setBold(true);
//        h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//        h1->aligment = Qt::AlignCenter;
//        addField(h1);  //Append field
//    }
//    if (isRowTotalVisible() && isColTotalVisible()) {
//        RptFieldObject *h1 = new RptFieldObject();
//        h1->name = QString("BottomRigthCorner");
//        h1->fieldType = Text;
//        h1->rect.setTop(rect.top() + fieldheight*(rowDataCount()+1) );
//        h1->rect.setLeft(rect.left() + fieldWidth*(colDataCount()+1));
//        h1->rect.setHeight(fieldheight);
//        h1->rect.setWidth(fieldWidth);
//        h1->value = QString("");
//        h1->font.setBold(true);
//        h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//        h1->aligment = Qt::AlignCenter;
//        addField(h1);  //Append field
//    }

//    for (int row=-1; row < rowDataCount(); row++) {
//        //Make a rowHeader
//        if (row >= 0 && isRowHeaderVisible()) {
//            RptFieldObject *h1 = new RptFieldObject();
//            h1->name = QString("rh%1").arg(row);
//            h1->fieldType = Text;
//            h1->rect.setTop(rect.top() + fieldheight*(row+1) );
//            h1->rect.setLeft(rect.left());
//            h1->rect.setHeight(fieldheight);
//            h1->rect.setWidth(fieldWidth);
//            h1->value = "r-"+m_rowHeader[row];
//            h1->font.setBold(true);
//            h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//            h1->aligment = Qt::AlignVCenter | Qt::AlignLeft;
//            addField(h1);  //Append field
//        }
//        //Last column in row
//        if (isRowTotalVisible()) {
//            if ((row == -1 && isColHeaderVisible()) || row >= 0) {
//                //Col of total - Total by row
//                RptFieldObject *h1 = new RptFieldObject();
//                h1->name = QString("t%1").arg(row);
//                h1->fieldType = Text;
//                h1->rect.setTop(rect.top() + fieldheight*(row+1));
//                h1->rect.setLeft(rect.left() + fieldWidth*(colDataCount()+1) );
//                h1->rect.setHeight(fieldheight);
//                h1->rect.setWidth(fieldWidth);
//                if (row == -1)
//                    h1->value = QString("C-Total");
//                else
//                    h1->value = QString("c%1").arg(row);
//                h1->font.setBold(true);
//                h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//                h1->aligment = Qt::AlignCenter;
//                addField(h1);  //Append field
//            }
//        }
//    }

//    for (int col=-1; col < colDataCount(); col++) {
//        if (col >=0 && isColHeaderVisible()) {
//            //Make a colHeader
//            RptFieldObject *h1 = new RptFieldObject();
//            h1->name = QString("ch%1").arg(col);
//            h1->fieldType = Text;
//            h1->rect.setTop(rect.top());
//            h1->rect.setLeft(rect.left() + fieldWidth*(col+1) );
//            h1->rect.setHeight(fieldheight);
//            h1->rect.setWidth(fieldWidth);
//            h1->value = m_colHeader[col];
//            h1->font.setBold(true);
//            h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//            h1->aligment = Qt::AlignCenter;
//            addField(h1);  //Append field
//        }
//        //Last row in column
//        if (isColTotalVisible()) {
//             if ((col == -1 && isRowHeaderVisible()) || col >= 0) {
//                 //Row of total - Total by column
//                 RptFieldObject *h1 = new RptFieldObject();
//                 h1->name = QString("t%1").arg(col);
//                 h1->fieldType = Text;
//                 h1->rect.setTop(rect.top() + fieldheight*(rowDataCount()+1));
//                 h1->rect.setLeft(rect.left() + fieldWidth*(col+1) );
//                 h1->rect.setHeight(fieldheight);
//                 h1->rect.setWidth(fieldWidth);
//                 if (col == -1) {
//                     h1->value = QObject::tr("R-Total");
//                     h1->aligment = Qt::AlignVCenter | Qt::AlignLeft;
//                 } else {
//                    h1->value = QString("t%1").arg(col);
//                    h1->aligment = Qt::AlignVCenter | Qt::AlignRight;
//                 }
//                 h1->font.setBold(true);
//                 h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//                 addField(h1);  //Append field
//             }
//         }
//    }

//    //Fill values
//    for (int row=0; row < rowDataCount(); row++) {
//        for (int col=0; col < colDataCount(); col++) {
//            RptFieldObject *h1 = new RptFieldObject();
//            h1->name = QString("f%1%2").arg(col).arg(row);
//            h1->fieldType = Text;
//            h1->rect.setTop(rect.top() + fieldheight*(row+1) );
//            h1->rect.setLeft(rect.left() + fieldWidth*(col+1) );
//            h1->rect.setHeight(fieldheight);
//            h1->rect.setWidth(fieldWidth);
//            h1->value =  getMatrixValue(col,row).toString();
//            h1->font.setBold(true);
//            //h1->setDefaultBackgroundColor(Qt::lightGray); //Set default background color
//            h1->aligment = Qt::AlignCenter;
//            addField(h1);  //Append field
//        }
//    }
}

/*!
 \fn void RptCrossTabObject::addField(RptFieldObject *field)
    Add \a field to current RptCrossTabObject object.

    \sa RptFieldObject
*/
void RptCrossTabObject::addField(RptFieldObject *field) {
    //field->parentBand = this;
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

//    if (obj.isColHeaderVisible()) {
//        dbg << "\t";
//        for(int col=0; col<obj.colCount(); col++) {
//            dbg << "|" << obj.getColName(col) << "\t";
//        }
//        dbg << "\n";
//    }

//    for(int row=0; row<obj.rowCount(); row++) {
//        if (obj.isRowHeaderVisible()) {
//            dbg << obj.getRowName(row);
//            dbg << "\t";
//        }

//        for(int col=0; col<obj.colCount(); col++) {
//            dbg << "|" << obj.getMatrixValue(col,row).toString() << "\t";
//        }
//        dbg << "\n";
//    }
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


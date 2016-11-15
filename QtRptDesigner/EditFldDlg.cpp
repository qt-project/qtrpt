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

#include "EditFldDlg.h"
#include "ui_EditFldDlg.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

EditFldDlg::EditFldDlg(QWidget *parent) :  QDialog(parent), ui(new Ui::EditFldDlg) {
    ui->setupUi(this);

    QObject::connect(ui->btnLoadImage, SIGNAL(clicked()), this, SLOT(loadImage()));
    QObject::connect(ui->btnSaveImage, SIGNAL(clicked()), this, SLOT(saveImage()));
    QObject::connect(ui->btnAddVariable, SIGNAL(clicked()), this, SLOT(openProperty()));
    QObject::connect(ui->btnFormatting, SIGNAL(clicked()), this, SLOT(openProperty()));
    QObject::connect(ui->btnAddFunction, SIGNAL(clicked()), this, SLOT(openProperty()));
    QObject::connect(ui->rdPrinting, SIGNAL(toggled(bool)), this, SLOT(conditionalToggled(bool)));
    QObject::connect(ui->rdTransparent, SIGNAL(toggled(bool)), this, SLOT(backGroundToggled(bool)));
    QObject::connect(ui->edtCondition, SIGNAL(textEdited(const QString&)), this, SLOT(conditionChanged(const QString&)));
    QObject::connect(ui->btnColorB, SIGNAL(clicked()), this, SLOT(chooseColor()));
    QObject::connect(ui->btnColorF, SIGNAL(clicked()), this, SLOT(chooseColor()));
    QObject::connect(ui->chkBold, SIGNAL(clicked()), this, SLOT(encodeHighLightingString()));
    QObject::connect(ui->chkItalic, SIGNAL(clicked()), this, SLOT(encodeHighLightingString()));
    QObject::connect(ui->chkUnderline, SIGNAL(clicked()), this, SLOT(encodeHighLightingString()));
    QObject::connect(ui->chkStrikeout, SIGNAL(clicked()), this, SLOT(encodeHighLightingString()));
    QObject::connect(ui->chkGraphs, SIGNAL(toggled(bool)), this, SLOT(autoFillData(bool)));
}

void EditFldDlg::chooseColor() {
    QColor color;
    if (sender() == ui->btnColorF)
        color = colorFromString(ui->lblColorF->styleSheet());
    if (sender() == ui->btnColorB)
        color = colorFromString(ui->lblColorB->styleSheet());
    QColorDialog *dlg = new QColorDialog(color, this);
    if (dlg->exec() == QDialog::Accepted) {
        color = dlg->selectedColor();
    } else return;

    QString strColor = colorToString(color);
    if (sender() == ui->btnColorB)
        ui->lblColorB->setStyleSheet("QLabel {background-color: "+strColor+"}");
    if (sender() == ui->btnColorF)
        ui->lblColorF->setStyleSheet("QLabel {background-color: "+strColor+"}");

    encodeHighLightingString();
}

void EditFldDlg::conditionChanged(const QString &text) {
    if (ui->rdPrinting->isChecked())
        m_cond_printing = text;
    if (ui->rdHighlighting->isChecked())
        encodeHighLightingString();
}

//Switching between Printing and Highlighting
void EditFldDlg::conditionalToggled(bool value) {
    ui->grpBackground->setEnabled(!value);
    ui->grpFont->setEnabled(!value);

    if (value) { //Show printting condition
        encodeHighLightingString();
        ui->edtCondition->setText(m_cond_printing);
    } else {  //Show highlighting condtion
        m_cond_printing = ui->edtCondition->text();
        decodeHighLightingString();
        ui->edtCondition->setText(m_cond_higlighting.section(";",0,0));
    }
    backGroundToggled(!false);
}

void EditFldDlg::decodeHighLightingString() {
    for(auto str : m_cond_higlighting.split(";")) {
        if (str.contains("bold")) ui->chkBold->setChecked(true);
        if (str.contains("italic")) ui->chkItalic->setChecked(true);
        if (str.contains("underline")) ui->chkUnderline->setChecked(true);
        if (str.contains("strikeout")) ui->chkStrikeout->setChecked(true);
        if (str.contains("fontColor")) {
            int start = str.indexOf("rgba(",0,Qt::CaseInsensitive);
            ui->lblColorF->setStyleSheet("QLabel {background-color: "+str.mid(start)+"}");
        }
        if (str.contains("backgroundColor")) {
            int start = str.indexOf("rgba(",0,Qt::CaseInsensitive);
            ui->lblColorB->setStyleSheet("QLabel {background-color: "+str.mid(start)+"}");
            ui->rdOther->setChecked(true);
            backGroundToggled(false);
        }
    }
}

void EditFldDlg::encodeHighLightingString() {
    if (ui->edtCondition->text().isEmpty()) return;
    if (ui->rdPrinting->isChecked()) return;
    m_cond_higlighting.clear();

    QString tmpStr = ui->edtCondition->text();
    m_cond_higlighting = tmpStr+";"; //First item in section is always a condition

    if (ui->chkBold->isChecked())
        m_cond_higlighting  += "bold=?1:0;";
    if (ui->chkItalic->isChecked())
        m_cond_higlighting += "italic=?1:0;";
    if (ui->chkUnderline->isChecked())
        m_cond_higlighting += "underline=?1:0;";
    if (ui->chkStrikeout->isChecked())
        m_cond_higlighting += "strikeout=?1:0;";

    int startF = ui->lblColorF->styleSheet().indexOf("rgba(",0,Qt::CaseInsensitive);
    int endF = ui->lblColorF->styleSheet().indexOf(")",Qt::CaseInsensitive)+1;
    int startB = ui->lblColorB->styleSheet().indexOf("rgba(",0,Qt::CaseInsensitive);
    int endB = ui->lblColorB->styleSheet().indexOf(")",Qt::CaseInsensitive)+1;

    QString strColorF = ui->lblColorF->styleSheet().mid(startF,endF-startF);
    QString strColorB = ui->lblColorB->styleSheet().mid(startB,endB-startB);

    if (!strColorF.isEmpty() && strColorF != "255,255,255,255" && strColorF != "255,255,255,0") {
        m_cond_higlighting += "fontColor=?"+strColorF+";";
    }
    if (ui->rdOther->isChecked()) {
        if (!strColorB.isEmpty() && strColorB != "255,255,255,255" && strColorB != "255,255,255,0") {
            m_cond_higlighting += "backgroundColor=?"+strColorB+";";
        }
    }
}

void EditFldDlg::backGroundToggled(bool value) {
    ui->lblColorB->setEnabled(!value);
    ui->btnColorB->setEnabled(!value);
    if (ui->rdHighlighting->isChecked())
        encodeHighLightingString();
}

void EditFldDlg::openProperty() {
    FldPropertyDlg *dlg = new FldPropertyDlg(this);
    if (sender() == ui->btnAddVariable) {
        QString str = dlg->showThis(0,0,"");
        ui->textEdit->insertPlainText(str);
    }
    if (sender() == ui->btnAddFunction) {
        QString str = dlg->showThis(3,0,"");
        ui->textEdit->insertPlainText(str);
    }
    if (sender() == ui->btnFormatting) {
         QString str = dlg->showThis(2, 0, m_cont->getFormatString());
         m_cont->setFormatString(str);
    }
    delete dlg;
}

void EditFldDlg::textDirection() {
    QTextOption topt = ui->textEdit->document()->defaultTextOption();
    if (ui->btnTextDirection->isChecked()) {
        topt.setTextDirection(Qt::RightToLeft);
    } else {
        topt.setTextDirection(Qt::LeftToRight);
    }
    ui->textEdit->document()->setDefaultTextOption(topt);
}

int EditFldDlg::showText(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);
    ui->textEdit->setPlainText(cont->getText());
    ui->stackedWidget->setCurrentIndex(0);
    ui->textEdit->setFocus();
    m_cont = cont;

    ui->btnTextDirection->setEnabled(true);
    QObject::connect(ui->btnTextDirection, SIGNAL(clicked()), this, SLOT(textDirection()));

    if (cont->getTextDirection())
        ui->btnTextDirection->click();

    switch (cont->getFieldType()) {
        case TextImage: {
            ui->radioButtonTextImage->setChecked(true);
            boolImage = true;
            break;
        }
        case DatabaseImage: {
            ui->radioButtonDatabaseImage->setChecked(true);
            boolImage = true;
            break;
        }
        default: {
            ui->radioButtonText->setChecked(true);
            boolImage = false;
            break;
        }
    }

    ui->lblAttention->setVisible(boolImage);
    ui->rdPrinting->setChecked(true);

    if (cont->getPrinting().size() > 1) {
        m_cond_printing = cont->getPrinting().replace("?1:0","");
        ui->edtCondition->setText(m_cond_printing);
    }
    if (cont->getHighlighting().size() > 0) {
        m_cond_higlighting = cont->getHighlighting();
        decodeHighLightingString();
    }

    if (this->exec()) {
        if (ui->radioButtonTextImage->isChecked()) cont->setFieldType(TextImage);
        else if (ui->radioButtonDatabaseImage->isChecked()) cont->setFieldType(DatabaseImage);
        else cont->setFieldType(Text);

        QString plainTextEditContents = ui->textEdit->toPlainText();
        QStringList lines = plainTextEditContents.split("\n");

        if (lines.count() > 1 && lines.last().trimmed().isEmpty()) {
            QMessageBox::StandardButton reply;            
            reply = QMessageBox::question(this, tr("Empty line"),tr("The field contains empty line at the end.\nRemove it?"),
                                             QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                plainTextEditContents.clear();
                //Remove last empty lines
                int i = lines.count()-1;
                while (i>0) {
                    if (lines.last().trimmed().isEmpty() && i==lines.count()-1)
                        lines.removeAt(lines.count()-1);
                    else i--;
                }
                //join all in one string
                for (int i = 0; i < lines.size(); ++i) {
                    plainTextEditContents += lines.at(i);
                    if (i!=lines.size()-1)
                        plainTextEditContents += "\n";
                }
            }
        }

        if (cont->getTextDirection() != ui->btnTextDirection->isChecked()) {
            if (ui->btnTextDirection->isChecked())
                cont->setAlignment(Qt::AlignRight);
            else
                cont->setAlignment(Qt::AlignLeft);
        }
        cont->setTextDirection(ui->btnTextDirection->isChecked());

        cont->setText(plainTextEditContents);

        if (m_cond_printing.size() > 0)
            cont->setPrinting( m_cond_printing+"?1:0" );
        else {
            if (cont->getPrinting().size() > 1)  //If previous was a Formula, now just a Visible
                cont->setPrinting("1");
        }
        if (m_cond_higlighting.size() > 0) {
            cont->setHighlighting(m_cond_higlighting);
        } else
            cont->setHighlighting("");

        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

int EditFldDlg::showTextRich(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);
    ui->textEditRich->textEdit->setHtml(cont->getText());
    ui->stackedWidget->setCurrentIndex(4);
    ui->textEdit->setFocus();
    m_cont = cont;

    if (this->exec()) {
        QString plainTextEditContents = ui->textEditRich->textEdit->toHtml();
        QStringList lines = plainTextEditContents.split("\n");

        if (lines.count() > 1 && lines.last().trimmed().isEmpty()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Empty line"),tr("The field contains empty line at the end.\nRemove it?"),
                                             QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                plainTextEditContents.clear();
                //Remove last empty lines
                int i = lines.count()-1;
                while (i>0) {
                    if (lines.last().trimmed().isEmpty() && i==lines.count()-1)
                        lines.removeAt(lines.count()-1);
                    else i--;
                }
                //join all in one string
                for (int i = 0; i < lines.size(); ++i) {
                    plainTextEditContents += lines.at(i);
                    if (i!=lines.size()-1)
                        plainTextEditContents += "\n";
                }
            }
        }

        cont->setText(plainTextEditContents);

        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

int EditFldDlg::showImage(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);

    ui->stackedWidget->setCurrentIndex(1);
    ui->label->setPixmap(cont->getImage());
    QObject::connect(ui->chkIgnoreAspectRatio, SIGNAL(toggled(bool)), this, SLOT(setScaledContents(bool)));

    ui->chkIgnoreAspectRatio->setChecked(cont->getIgnoreAspectRatio());
    m_imgFormat = cont->getImgFormat();
    if (this->exec()) {
        cont->setIgnoreAspectRatio(ui->chkIgnoreAspectRatio->isChecked());
        cont->setImage(*ui->label->pixmap());
        cont->setImgFromat(m_imgFormat);

        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

int EditFldDlg::showBarcode(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);
    ui->stackedWidget->setCurrentIndex(3);
    QObject::connect(ui->edtValue, SIGNAL(textChanged(QString)), ui->wBarcode, SLOT(setValue(QString)));
    QObject::connect(ui->bstyle, SIGNAL(activated(int)), SLOT(update_preview()));
    QObject::connect(ui->cbFrameType, SIGNAL(activated(int)), SLOT(update_preview()));
    QObject::connect(ui->spnHeight, SIGNAL(valueChanged(int)), SLOT(update_preview()));
    ui->edtValue->setText(cont->getText());

    BarCode::BarcodeTypePairList list1 = BarCode::getTypeList();
    for (int i=0; i < list1.size(); i++) {
        ui->bstyle->addItem(list1.at(i).second,list1.at(i).first);
        if (list1.at(i).first == cont->getBarcodeType() )
            ui->bstyle->setCurrentIndex(i);
    }

    BarCode::FrameTypePairList list2 = BarCode::getFrameTypeList();
    for (int i=0; i < list2.size(); i++) {
        ui->cbFrameType->addItem(list2.at(i).second,list2.at(i).first);
        if (list2.at(i).first == cont->getBarcodeFrameType() )
            ui->cbFrameType->setCurrentIndex(i);
    }
    ui->spnHeight->setValue(cont->getBarcodeHeight());

    if (this->exec()) {
        cont->setText(ui->edtValue->text());
        cont->setBarcodeType((BarCode::BarcodeTypes)ui->wBarcode->metaObject()->enumerator(0).value(ui->bstyle->currentIndex()));
        cont->setBarcodeFrameType((BarCode::FrameTypes)ui->wBarcode->metaObject()->enumerator(1).value(ui->cbFrameType->currentIndex()));
        cont->setBarcodeHeight(ui->spnHeight->value());

        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

int EditFldDlg::showCrosstab(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);
    RptCrossTabObject *m_crossTab = cont->getCrossTab();
    if (m_crossTab == nullptr)
        return QDialog::Rejected;
    ui->stackedWidget->setCurrentIndex(5);
    ui->spnRowCount->setValue(m_crossTab->rowDataCount());
    ui->spnColCount->setValue(m_crossTab->colDataCount());
    ui->chkRowHeader->setChecked(m_crossTab->isRowHeaderVisible());
    ui->chkColHeader->setChecked(m_crossTab->isColHeaderVisible());
    ui->chkRowTotal->setChecked(m_crossTab->isRowTotalVisible());
    ui->chkColTotal->setChecked(m_crossTab->isColTotalVisible());
    ui->tblColHeaders->setRowCount(m_crossTab->colDataCount());
    ui->tblRowHeaders->setRowCount(m_crossTab->rowDataCount());
    for(int i=0; i<ui->tblColHeaders->rowCount(); i++) {
        QTableWidgetItem *newItem = new QTableWidgetItem(m_crossTab->getColName(i));
        ui->tblColHeaders->setItem(i,0,newItem);
    }
    for(int i=0; i<ui->tblRowHeaders->rowCount(); i++) {
        QTableWidgetItem *newItem = new QTableWidgetItem(m_crossTab->getRowName(i));
        ui->tblRowHeaders->setItem(i,0,newItem);
    }
    QObject::connect(ui->spnRowCount, SIGNAL(valueChanged(int)), SLOT(setCrossTabRowCount(int)));
    QObject::connect(ui->spnColCount, SIGNAL(valueChanged(int)), SLOT(setCrossTabColCount(int)));
    if (this->exec()) {
        m_crossTab->setRowHeaderVisible(ui->chkRowHeader->isChecked());
        m_crossTab->setColHeaderVisible(ui->chkColHeader->isChecked());
        m_crossTab->setRowTotalVisible(ui->chkRowTotal->isChecked());
        m_crossTab->setColTotalVisible(ui->chkColTotal->isChecked());
        m_crossTab->clear();
        for(int i=0; i<ui->tblColHeaders->rowCount(); i++)
            if (ui->tblColHeaders->item(i,0) == 0)
                m_crossTab->addCol("");
            else
                m_crossTab->addCol(ui->tblColHeaders->item(i,0)->text());

        for(int i=0; i<ui->tblRowHeaders->rowCount(); i++)
            if (ui->tblRowHeaders->item(i,0) == 0)
                m_crossTab->addRow("");
            else
                m_crossTab->addRow(ui->tblRowHeaders->item(i,0)->text());

        m_crossTab->initMatrix();
        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

void EditFldDlg::setCrossTabRowCount(int value) {
    ui->tblRowHeaders->setRowCount(value);
}

void EditFldDlg::setCrossTabColCount(int value) {
    ui->tblColHeaders->setRowCount(value);
}

void EditFldDlg::update_preview() {
    if (sender() == ui->bstyle)
        ui->wBarcode->setBarcodeType((BarCode::BarcodeTypes)ui->wBarcode->metaObject()->enumerator(0).value(ui->bstyle->currentIndex()));
    if (sender() == ui->cbFrameType)
        ui->wBarcode->setFrameType((BarCode::FrameTypes)ui->wBarcode->metaObject()->enumerator(1).value(ui->cbFrameType->currentIndex()));
    if (sender() == ui->spnHeight)
        ui->wBarcode->setHeight(ui->spnHeight->value());
}

void EditFldDlg::setScaledContents(bool value) {
    ui->label->setScaledContents(value);
}

int EditFldDlg::showDiagram(QGraphicsItem *gItem) {
    GraphicsBox *cont = static_cast<GraphicsBox*>(gItem);
    ui->stackedWidget->setCurrentIndex(2);
    ui->tabDiagram->setTabEnabled(1,false);
    ui->chkShowGrid->setChecked( cont->getChart()->getParam(DrawGrid).toBool() );
    ui->chkShowCaption->setChecked( cont->getChart()->getParam(ShowCaption).toBool() );
    ui->chkShowLegend->setChecked( cont->getChart()->getParam(ShowLegend).toBool() );
    ui->chkGraphsCaption->setChecked( cont->getChart()->getParam(ShowGraphCaption).toBool() );
    if (cont->getChart()->getParam(ShowPercent).toBool())
        ui->rbPercentValue->setChecked( cont->getChart()->getParam(ShowPercent).toBool() );
    else
        ui->rbRealValue->setChecked( cont->getChart()->getParam(ShowPercent).toBool() );
    ui->edtCaption->setText( cont->getChart()->getParam(Caption).toString() );
    ui->chkGraphs->setChecked( cont->getChart()->getParam(AutoFillData).toBool() );

    QObject::connect(ui->btnUp, SIGNAL(clicked()), this, SLOT(moveRow()));
    QObject::connect(ui->btnDown, SIGNAL(clicked()), this, SLOT(moveRow()));
    QObject::connect(ui->btnAddRow, SIGNAL(clicked()), this, SLOT(addRow()));
    QObject::connect(ui->btnRemoveRow, SIGNAL(clicked()), this, SLOT(removeRow()));
    QObject::connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

    QTableWidgetItem *newItem;
    int i=0;
    ui->tableWidget->setRowCount( cont->getChart()->getGraphParamList().size() );
    for(auto graphParam : cont->getChart()->getGraphParamList() ) {
        newItem = new QTableWidgetItem( graphParam.caption );
        ui->tableWidget->setItem(i,0,newItem);

        newItem = new QTableWidgetItem( graphParam.valueString );
        ui->tableWidget->setItem(i,1,newItem);

        newItem = new QTableWidgetItem( graphParam.valueString );
        ui->tableWidget->setItem(i,1,newItem);

        //-- color box and button for selecting color
        SelectColor *sc = new SelectColor(ui->tableWidget, colorToString(graphParam.color));
        QObject::connect(sc->button, SIGNAL(clicked()), this, SLOT(selectGraphColor()));
        ui->tableWidget->setCellWidget(i,2,sc);

        i++;
    }

    if (this->exec()) {
        //saving graphs to XML nodes
        cont->getChart()->clearData();
        for (int i=0; i<ui->tableWidget->rowCount(); i++) {
            GraphParam param;
            SelectColor *sc = qobject_cast<SelectColor *>(ui->tableWidget->cellWidget(i,2));
            param.color = colorFromString(sc->getBackGroundColor());
            param.valueReal = ui->tableWidget->item(i,1)->text().toFloat();
            param.valueString = ui->tableWidget->item(i,1)->text();
            param.caption = ui->tableWidget->item(i,0)->text();
            cont->getChart()->setData(param);
        }
        cont->getChart()->setParams(ui->chkShowGrid->isChecked(),
                                    ui->chkShowLegend->isChecked(),
                                    ui->chkShowCaption->isChecked(),
                                    ui->chkGraphsCaption->isChecked(),
                                    ui->rbPercentValue->isChecked(),
                                    ui->edtCaption->text(),
                                    ui->chkGraphs->isChecked());


        return QDialog::Accepted;
    } else return QDialog::Rejected;
}

void EditFldDlg::removeRow() {
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void EditFldDlg::addRow() {
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setCurrentCell(ui->tableWidget->rowCount()-1,0);
    QTableWidgetItem *newItem;

    newItem = new QTableWidgetItem("New graph");
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,newItem);

    newItem = new QTableWidgetItem("Field");
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,newItem);

    SelectColor *sc = new SelectColor(ui->tableWidget, "rgba(255,255,255,255)");
    QObject::connect(sc->button, SIGNAL(clicked()), this, SLOT(selectGraphColor()));
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,2,sc);
    ui->tableWidget->setFocus();
}

void EditFldDlg::itemSelectionChanged() {
    if (ui->tableWidget->rowCount() == 0) {
        ui->btnUp->setEnabled(false);
        ui->btnDown->setEnabled(false);
        ui->btnRemoveRow->setEnabled(false);
    }
    int row = ui->tableWidget->currentRow();
    if (row == 0 || ui->tableWidget->rowCount() == 1) ui->btnUp->setEnabled(false);
    else ui->btnUp->setEnabled(true);
    if (row == ui->tableWidget->rowCount()-1 || ui->tableWidget->rowCount() == 1) ui->btnDown->setEnabled(false);
    else ui->btnDown->setEnabled(true);
}

void EditFldDlg::moveRow() {
    const int row = ui->tableWidget->currentRow();
    const int col = ui->tableWidget->currentColumn();
    QTableWidgetItem *newItem1 = ui->tableWidget->takeItem(ui->tableWidget->currentRow(),0);
    QTableWidgetItem *newItem2 = ui->tableWidget->takeItem(ui->tableWidget->currentRow(),1);
    QWidget *newItem3 = ui->tableWidget->cellWidget(ui->tableWidget->currentRow(),2);

    if (sender() == ui->btnUp) { //up
        ui->tableWidget->insertRow(row-1);
        ui->tableWidget->setItem(row-1,0,newItem1);
        ui->tableWidget->setItem(row-1,1,newItem2);
        ui->tableWidget->setCellWidget(row-1,2,newItem3);
        ui->tableWidget->setCurrentCell(row-1,col);
        ui->tableWidget->removeRow(row+1);
    }
    if (sender() == ui->btnDown) { //down
        ui->tableWidget->insertRow(row+2);
        ui->tableWidget->setItem(row+2,0,newItem1);
        ui->tableWidget->setItem(row+2,1,newItem2);
        ui->tableWidget->setCellWidget(row+2,2,newItem3);
        ui->tableWidget->setCurrentCell(row+2,col);
        ui->tableWidget->removeRow(row);
    }
}

void EditFldDlg::selectGraphColor() {
    QColor color;
    QColorDialog *dlg = new QColorDialog(color, this);
    if (dlg->exec() == QDialog::Accepted) {
        color = dlg->selectedColor();
    } else return;

    QString strColor = colorToString(color);

    QWidget *colorBox = ui->tableWidget->cellWidget(ui->tableWidget->currentRow(),2)->findChild<QWidget *>("colorBox");

    int start; int end;
    QString str = colorBox->styleSheet();
    start = str.indexOf(";background-color:",0,Qt::CaseInsensitive);
    end = str.indexOf(";",start+1,Qt::CaseInsensitive);
    str.replace(start,end-start,";background-color:"+strColor);
    colorBox->setStyleSheet(str);
}

void EditFldDlg::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QPixmap p;
        p.load(fileName);        
        ui->label->setPixmap(p);
        ui->label->setScaledContents(ui->chkIgnoreAspectRatio->isChecked());
        QFileInfo fi(fileName);
        m_imgFormat = fi.suffix().toUpper();
    }
}

void EditFldDlg::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image As"),
                                                    QCoreApplication::applicationDirPath(),
                                                    tr("Images (*.png)"));
    if (!fileName.isEmpty()) {
        QPixmap p = QPixmap(*ui->label->pixmap());
        if (p.isNull()) return;
        p.save(fileName, m_imgFormat.toLatin1().data());
    }
}

void EditFldDlg::autoFillData(bool value) {
    ui->tabDiagram->setTabEnabled(1,value);
}

EditFldDlg::~EditFldDlg() {
    delete ui;
}



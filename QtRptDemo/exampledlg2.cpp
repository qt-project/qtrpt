/*
Name: QtRpt
Version: 1.5.5
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
2012-2015

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

#include "exampledlg2.h"
#include "ui_exampledlg2.h"
#include <QDir>
#include <QDebug>

ExampleDlg2::ExampleDlg2(QWidget *parent) : QDialog(parent), ui(new Ui::ExampleDlg2) {
    ui->setupUi(this);
    QObject::connect(ui->btnPrint, SIGNAL(clicked()), this, SLOT(print()));
    ui->table1->setRowCount(60);
    ui->table2->setRowCount(55);

    QTableWidgetItem *newItem;
    for (int i = 0; i < ui->table1->rowCount(); ++i) {
        newItem = new QTableWidgetItem("DS1: "+QString::number(i+1));
        ui->table1->setItem(i,0,newItem);
    }
    for (int i = 0; i < ui->table2->rowCount(); ++i) {
        newItem = new QTableWidgetItem("DS2: "+QString::number(i+1));
        ui->table2->setItem(i,0,newItem);
    }
}

void ExampleDlg2::print() {
    QDir dir(qApp->applicationDirPath());
    #if defined(Q_OS_MAC)
        dir.cd(QFile::decodeName("../Resources"));
    #endif

    QString fileName = dir.absolutePath()+"/examples_report/example2.xml";
    report = new QtRPT(this);
    report->recordCount << ui->table1->rowCount();
    report->recordCount << ui->table2->rowCount();
    if (report->loadReport(fileName) == false) {
        qDebug()<<"Report file not found";
    }
    QObject::connect(report, SIGNAL(setValue(const int, const QString, QVariant&, const int)),
                     this, SLOT(setValue(const int, const QString, QVariant&, const int)));
    report->printExec();

}

void ExampleDlg2::setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage) {
    if (paramName == "value") {
        if (reportPage == 0) {
            if (ui->table1->item(recNo,0) == 0) return;
            paramValue = ui->table1->item(recNo,0)->text();
        }
        if (reportPage == 1) {
            if (ui->table2->item(recNo,0) == 0) return;
            paramValue = ui->table2->item(recNo,0)->text();
        }
    }
}

ExampleDlg2::~ExampleDlg2() {
    delete ui;
}

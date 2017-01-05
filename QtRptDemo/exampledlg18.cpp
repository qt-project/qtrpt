/*
Name: QtRpt
Version: 2.0.0
Programmer: Aleksey Osipov
E-mail: aliks-os@ukr.net
2012-2016

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

#include "exampledlg18.h"
#include "ui_exampledlg18.h"
#include <QDebug>

ExampleDlg18::ExampleDlg18(QWidget *parent) : QDialog(parent), ui(new Ui::ExampleDlg18) {
    ui->setupUi(this);
    QObject::connect(ui->btnPrint, SIGNAL(clicked()), this, SLOT(print()));
}

void ExampleDlg18::print() {
    QDir dir(qApp->applicationDirPath());
    #if defined(Q_OS_MAC)
        dir.cd(QFile::decodeName("../Resources"));
    #endif

    QString fileName = dir.absolutePath()+"/examples_report/example18.xml";
    report = new QtRPT(this);
//    report->setBackgroundImage(QPixmap(dir.absolutePath()+"/examples_report/qt_background_portrait.png"));
//    report->recordCount << ui->tableWidget->rowCount();
    if (report->loadReport(fileName) == false) {
        qDebug()<<"Report file not found";
    }
//    QObject::connect(report, SIGNAL(setValue(const int, const QString, QVariant&, const int)),
//                     this, SLOT(setValue(const int, const QString, QVariant&, const int)));
//    QObject::connect(report, SIGNAL(setValueImage(const int, const QString, QImage&, const int)),
//                     this, SLOT(setValueImage(const int, const QString, QImage&, const int)));
    //report->setCallbackFunc(getReportValue);
    report->printExec(true);
}

ExampleDlg18::~ExampleDlg18() {
    delete ui;
}

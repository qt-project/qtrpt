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

#include "aboutDlg.h"

AboutDlg::AboutDlg(QWidget *parent) : QDialog(parent) {
    this->setWindowTitle(tr("About QtRptDesigner"));
    QSize MaxSize(420, 450);
    QSize MinSize(420, 450);
    setMaximumSize(MaxSize);
    setMinimumSize(MinSize);

    QRect gry = geometry();
    gry.moveCenter(qApp->desktop()->availableGeometry().center());
    setGeometry(gry);

    QPushButton *btnOK = new QPushButton(this);
    btnOK->setText("OK");
    QObject::connect(btnOK, SIGNAL(clicked()), this, SLOT(close()));

    QLabel *labImg = new QLabel(this);
    labImg->setPixmap(QPixmap(":/new/prefix1/images/Logo128.png"));
    QString lbl1 = "<h2><b><p style='color:#0033FF'>"+QApplication::applicationName()+"</p></b></h2>"+
                  tr("Version: ")+QApplication::applicationVersion()+"<br>"+
                  tr("Programmer: Aleksey Osipov")+"<br>"+
                  "WebSite: <a href='http://www.aliks-os.tk'>http://www.aliks-os.tk</a>"+"<br>"+
                  "E-mail: aliks-os@ukr.net"+"<br>"+
                  "<a href='http://www.qtrpt.tk'>http://www.qtrpt.tk</a>"+"<br>"+
                  tr("2012-2015 years")+"<br><br>";
    QString lbl2 = "<b>"+tr("Thanks for donation:")+"</b>"+
                  "<ul>"+
                  "<li>"+tr("Sailendram")+"</li>"+
                  "</ul>"+
                  "<b>"+tr("Thanks for project developing:")+"</b>"+
                  "<ul>"+
                  "<li>"+tr("Lukas Lalinsky for DBmodel")+"</li>"+
                  "<li>"+tr("Norbert Schlia for help in developing")+"</li>"+
                  "<li>"+tr("Muhamad Bashir Al-Noimi for Arabic translation")+"</li>"+
                  "<li>"+tr("Luis Brochado for Portuguese translation")+"</li>"+
                  "<li>"+tr("Li Wei for Chinese translation")+"</li>"+
                  "<li>"+tr("Laurent Guilbert for French translation")+"</li>"+
                  "<li>"+tr("David Heremans for Dutch translation")+"</li>"+
                  "<li>"+tr("Mirko Marx for German translation")+"</li>"+
                  "<li>"+tr("Manuel Soriano for Spanish translation")+"</li>"+
                  "</ul>";
    QLabel *lab1 = new QLabel(lbl1, this);
    QObject::connect(lab1, SIGNAL(linkActivated(const QString)), this, SLOT(openLink(const QString)));
    QLabel *lab2 = new QLabel(lbl2, this);

    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(labImg);
    hLayout2->addWidget(lab1);
    hLayout2->addStretch();

    QHBoxLayout *hLayout1 = new QHBoxLayout;
    //hLayout->addSpacerItem(spacer1);
    hLayout1->addStretch();
    hLayout1->addWidget(btnOK);
    //hLayout->addSpacerItem(spacer2);
    hLayout1->addStretch();

    QVBoxLayout *vLayout = new QVBoxLayout;
    //vLayout->addSpacerItem(spacer3);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(lab2);
    vLayout->addStretch();
    vLayout->addLayout(hLayout1);
    this->setLayout(vLayout);
}

void AboutDlg::openLink(const QString url) {
    QDesktopServices::openUrl(QUrl(url));
}

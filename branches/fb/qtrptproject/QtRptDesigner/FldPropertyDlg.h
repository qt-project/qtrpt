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

#ifndef FLDPROPERTYDLG_H
#define FLDPROPERTYDLG_H

#include <QDialog>

namespace Ui {
class FldPropertyDlg;
}

class FldPropertyDlg : public QDialog
{
    Q_OBJECT
    
public:
    explicit FldPropertyDlg(QWidget *parent = 0);
    ~FldPropertyDlg();
    QString showThis(int index, QWidget *widget = 0, QString value = 0);
    
private:
    Ui::FldPropertyDlg *ui;

private slots:
    void changeCategory(int row);
    void changeFormat(int row);
    void precisionChanged(int row);

};

#endif // FLDPROPERTYDLG_H

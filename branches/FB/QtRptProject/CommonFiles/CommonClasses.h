/*
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

#ifndef COMMONCLASSES_H
#define COMMONCLASSES_H

#include <QMap>
#include <QMetaEnum>
#include <QEvent>
#if QT_VERSION >= 0x50000
    #include <QtMath>
#else
    #include <QtCore/qmath.h>
#endif
#include <QObject>
#include <QWidget>

QString double2MoneyUKR(double n, int currency = 1);
QString double2MoneyRUS(double n, int currency = 2);
QString double2MoneyGER(double number, bool bAdditional = false);
QString double2MoneyENG(double number);
QString double2Money(double n, QString lang);
QString colorToString(QColor color);
QColor colorFromString(QString value);
QString eventType(QEvent *ev);

#endif // COMMONCLASSES_H

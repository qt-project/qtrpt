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

#ifndef RPTPAGEOBJECT_H
#define RPTPAGEOBJECT_H

#include <qtrptnamespace.h>
#include <RptBandObject.h>
#include <RptFieldObject.h>

using namespace QtRptName;

class QtRPT;
class RptBandObject;
class RptFieldObject;

class RptPageObject
{
    friend class QtRPT;
public:
    RptPageObject();
    ~RptPageObject();
    int pageNo;
    int orientation;
    int ph;
    int pw;
    int ml;
    int mr;
    int mt;
    int mb;
    bool border;
    int borderWidth;
    QColor borderColor;
    QString borderStyle;
    void addBand(RptBandObject *band);
    RptBandObject *getBand(BandType type);
    RptFieldObject *findFieldObjectByName(QString name);
    QList<RptBandObject*> bandList;

private:
    void setProperty(QtRPT *qtrpt, QDomElement docElem);
};
Q_DECLARE_METATYPE(RptPageObject)
QDebug operator<<(QDebug dbg, const RptPageObject &obj);

#endif // RPTPAGEOBJECT_H

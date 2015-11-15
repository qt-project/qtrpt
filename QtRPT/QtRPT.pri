QT       += gui xml script sql

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets printsupport
    DEFINES += HAVE_QT5
}

#DEFINES += NO_BARCODE
include(../CommonFiles/CommonFiles_QtRpt.pri)

INCLUDEPATH += $$PWD
#INCLUDEPATH += $$PWD/../3rdparty/QtXlsx
#LIBS += -L$${DEST_DIRECTORY}/lib -lQtXlsx

SOURCES += $$PWD/qtrpt.cpp \
           $$PWD/RptSql.cpp \
           $$PWD/RptSqlConnection.cpp \
           $$PWD/RptFieldObject.cpp \
           $$PWD/RptBandObject.cpp \
           $$PWD/RptPageObject.cpp \
           $$PWD/RptCrossTabObject.cpp
HEADERS += $$PWD/qtrpt.h \
           $$PWD/qtrptnamespace.h \
           $$PWD/RptSql.h \
           $$PWD/RptSqlConnection.h \
           $$PWD/RptFieldObject.h \
           $$PWD/RptBandObject.h \
           $$PWD/RptPageObject.h \
           $$PWD/RptCrossTabObject.h

RESOURCES += \
    $$PWD/../QtRPT/imagesRpt.qrc

INCLUDEPATH += $$PWD

include($$PWD/../QtRPT/config.pri)

!contains(DEFINES,NO_BARCODE) {
    INCLUDEPATH += $$PWD/../zint-2.4.4/backend_qt4
    INCLUDEPATH += $$PWD/../zint-2.4.4/backend

    LIBS += -L$${DEST_DIRECTORY}/lib -lQtZint
}

SOURCES += $$PWD/CommonClasses.cpp \
           $$PWD/chart.cpp \
           $$PWD/Barcode.cpp

HEADERS += $$PWD/CommonClasses.h \
           $$PWD/chart.h \
           $$PWD/Barcode.h

#-------------------------------------------------
#
# Project created by QtCreator 2012-05-21T15:13:27
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets printsupport
    DEFINES += HAVE_QT5
}

TARGET = QtRptDemo
TEMPLATE = app

contains(DEFINES,QTRPT_LIBRARY) {
    INCLUDEPATH += $$PWD/../QtRPT/
    LIBS += -L$${DEST_DIRECTORY}/lib -lQtRPT
}

include(../QtRPT/QtRPT.pri)
DESTDIR = $${DEST_DIRECTORY}

SOURCES += main.cpp\
    mainwindow.cpp \
    exampledlg1.cpp \
    exampledlg3.cpp \
    exampledlg2.cpp \
    exampledlg4.cpp \
    exampledlg5.cpp \
    exampledlg6.cpp \
    exampledlg7.cpp \
    exampledlg8.cpp \
    exampledlg13.cpp \
    exampledlg14.cpp

HEADERS  += mainwindow.h \
    exampledlg1.h \
    exampledlg3.h \
    exampledlg2.h \
    exampledlg4.h \
    exampledlg5.h \
    exampledlg6.h \
    exampledlg7.h \
    exampledlg8.h \
    exampledlg13.h \
    exampledlg14.h

FORMS    += mainwindow.ui \
    exampledlg1.ui \
    exampledlg3.ui \
    exampledlg2.ui \
    exampledlg4.ui \
    exampledlg5.ui \
    exampledlg6.ui \
    exampledlg7.ui \
    exampledlg8.ui \
    exampledlg13.ui \
    exampledlg14.ui

RESOURCES += \
    resurce.qrc

CONFIG += app_bundle

# Copy example files
win32 {
    QMAKE_LIB_DESTDIR       = $$quote($$DEST_DIRECTORY/)
    QMAKE_LIB_TARGET        = $${replace(QMAKE_LIB_DESTDIR, /, \\)}

    QMAKE_POST_LINK += \
        xcopy /sy \"$$PWD\\examples\\*\" \"$${QMAKE_LIB_TARGET}\"
}
else {
    QMAKE_LIB_DESTDIR       = $$quote($$DEST_DIRECTORY/)
    QMAKE_LIB_TARGET        = $$QMAKE_LIB_DESTDIR
    QMAKE_POST_LINK += \
        cp -rpv \"$$PWD/examples/\"* \"$${QMAKE_LIB_TARGET}\" $$escape_expand(\n\t)
}

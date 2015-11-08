QT       += core sql gui uitools printsupport xml script scripttools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtXlsx
TEMPLATE = lib
CONFIG += dll

macx{
    CONFIG -= dll
    CONFIG += lib_bundle
    CONFIG += plugin
}

linux{
    CONFIG += plugin
}

QMAKE_CXXFLAGS += -std=c++0x

include($$PWD/../../QtRPT/config.pri)  #Loading config file to get path where place ready library
!exists($$PWD/../../QtRPT/config.pri) {
    message("not found config.pri")
}

INCLUDEPATH += QtXlsx
DLLDESTDIR = $${DEST_DIRECTORY}
DESTDIR    = $${DEST_DIRECTORY}/lib


QMAKE_DOCS = $$PWD/doc/qtxlsx.qdocconf

#CONFIG += build_xlsx_lib
include(QtXlsx.pri)

QMAKE_TARGET_COMPANY = "Debao Zhang"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2013-2014 Debao Zhang <hello@debao.me>"
QMAKE_TARGET_DESCRIPTION = ".Xlsx file wirter for Qt5"


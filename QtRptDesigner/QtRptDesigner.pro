#-------------------------------------------------
#
# Project created by QtCreator 2012-03-29T16:09:02
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4){
    QT += widgets
    DEFINES += HAVE_QT5
}

TARGET = QtRptDesigner
VERSION = 1.5.5
DEFINES += VERSION=\\\"$$VERSION\\\"
TEMPLATE = app

include(../CommonFiles/CommonFiles_QtRptDesigner.pri)
include(../QtRPT/QtRPT.pri)
include(SQLDiagram/SQLDiagram.pri)

DESTDIR = $${DEST_DIRECTORY}

SOURCES += main.cpp\
    mainwindow.cpp \
    PageSettingDlg.cpp \
    ReportBand.cpp \
    RptContainer.cpp \
    EditFldDlg.cpp \
    aboutDlg.cpp \
    FldPropertyDlg.cpp \
    RepScrollArea.cpp \
    SettingDlg.cpp \
    overlay.cpp \
    SqlDesigner.cpp \
    TContainerLine.cpp \
    TContainerField.cpp \
    UndoCommands.cpp \
    XmlViewModel.cpp

HEADERS  += mainwindow.h \
    PageSettingDlg.h \
    ReportBand.h \
    RptContainer.h \
    EditFldDlg.h \
    aboutDlg.h \
    FldPropertyDlg.h \
    RepScrollArea.h \
    SettingDlg.h \
    overlay.h \
    SqlDesigner.h \
    TContainerLine.h \
    TContainerField.h \
    UndoCommands.h \
    XmlViewModel.h

FORMS    += mainwindow.ui \
    PageSettingDlg.ui \
    ReportBand.ui \
    EditFldDlg.ui \
    FldPropertyDlg.ui \
    RepScrollArea.ui \
    SettingDlg.ui \
    SqlDesigner.ui

RESOURCES += \
    images.qrc

RC_FILE = QtRptDesigner.rc

TRANSLATIONS +=  \
    $$PWD/i18n/QtRprtDesigner_uk_UA.ts \
    $$PWD/i18n/QtRprtDesigner_sr_RS.ts \
    $$PWD/i18n/QtRprtDesigner_sr_RS@latin.ts \
    $$PWD/i18n/QtRprtDesigner_ar.ts \
    $$PWD/i18n/QtRprtDesigner_ka_GE.ts \
    $$PWD/i18n/QtRprtDesigner_pt_PT.ts \
    $$PWD/i18n/QtRprtDesigner_zh_CN.ts \
    $$PWD/i18n/QtRprtDesigner_es_ES.ts \
    $$PWD/i18n/QtRprtDesigner_fr_FR.ts \
    $$PWD/i18n/QtRprtDesigner_de_DE.ts \
    $$PWD/i18n/QtRprtDesigner_nl_BE.ts \
    $$PWD/i18n/QtRprtDesigner_ta_IN.ts

OTHER_FILES += $$TRANSLATIONS

win32 {
    MOC_DIR = tmp-win32
    UI_DIR = tmp-win32
    UI_HEADERS_DIR = tmp-win32
    UI_SOURCES_DIR = tmp-win32
    OBJECTS_DIR = tmp-win32
    RCC_DIR = tmp-win32
}

unix {
    MOC_DIR = tmp-lin64
    UI_DIR = tmp-lin64
    UI_HEADERS_DIR = tmp-lin64
    UI_SOURCES_DIR = tmp-lin64
    OBJECTS_DIR = tmp-lin64
    RCC_DIR = tmp-lin64
}

CONFIG += c++11
CONFIG += app_bundle
CONFIG -= debug_and_release debug_and_release_target

# Automatically build required translation files (*.qm)
all.depends = locale
#QMAKE_EXTRA_TARGETS += all

QMAKE_CXXFLAGS += -std=c++0x

TRANSLATION_TARGETS = $$replace(TRANSLATIONS, "\.ts", ".qm")
locale.depends = $$TRANSLATION_TARGETS
QMAKE_EXTRA_TARGETS += locale

"%.qm".commands = lrelease -qm $@ $<
"%.qm".depends = "%.ts"
QMAKE_EXTRA_TARGETS += "%.qm"

PRE_TARGETDEPS += locale

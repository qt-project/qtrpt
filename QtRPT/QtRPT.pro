#Use this pro files only if you want to build QtRPT library
#and use it in your project

TARGET = QtRPT
TEMPLATE = lib
CONFIG += dll

DEFINES += QTRPT_LIBRARY

macx{
    CONFIG -= dll
    CONFIG += lib_bundle
    CONFIG += plugin
}

linux{
    CONFIG += plugin
}

HEADERS += \
    qtrpt_global.h

include(QtRPT.pri)
include(config.pri)

DLLDESTDIR = $${DEST_DIRECTORY}
DESTDIR    = $${DEST_DIRECTORY}/lib

win32 {
    MOC_DIR = tmp-win32
    #UI_DIR = tmp-win32
    #UI_HEADERS_DIR = tmp-win32
    #UI_SOURCES_DIR = tmp-win32
    OBJECTS_DIR = tmp-win32
    RCC_DIR = tmp-win32
}

linux {
    MOC_DIR = tmp-lin64
    #UI_DIR = tmp-lin64
    #UI_HEADERS_DIR = tmp-lin64
    #UI_SOURCES_DIR = tmp-lin64
    OBJECTS_DIR = tmp-lin64
    RCC_DIR = tmp-lin64
}



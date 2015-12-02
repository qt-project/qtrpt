TEMPLATE = subdirs
CONFIG += ordered


!contains(DEFINES,NO_BARCODE) {
    SUBDIRS += prj1
    prj1.file = $$PWD/zint-2.4.4/backend_qt4/Zint.pro
}

#SUBDIRS += $$PWD/QtXlsx/QtXlsx.pro

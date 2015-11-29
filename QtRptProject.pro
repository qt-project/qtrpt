TEMPLATE = subdirs

#DELETE NO_BARCODE    ---in this file
!contains(DEFINES,NO_BARCODE) {
    #SUBDIRS += prj1
}

#Un-remark this line, if you want to build QtRPT as a library
#DEFINES += QTRPT_LIBRARY
#contains(DEFINES,QTRPT_LIBRARY) {
#    SUBDIRS += QtRPT
#}

SUBDIRS += 3rdparty QtRptDesigner QtRptDemo #QtRptCGI
!contains(DEFINES,NO_BARCODE) {
    #prj1.file = $$PWD/zint-2.4.4/backend_qt4/Zint.pro
}

CONFIG += ordered



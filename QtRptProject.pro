TEMPLATE = subdirs
# Get top build dir for Qt4, see https://wiki.qt.io/QMake-top-level-srcdir-and-builddir
#lessThan(QT_MAJOR_VERSION, 5){
#    SUBDIRS = initvars.pro
#}
#else {
#    SUBDIRS =
#}
!contains(DEFINES,NO_BARCODE) {
    SUBDIRS += prj1
}

#Un-remark this line, if you want to build QtRPT as a library
# DEFINES += QTRPT_LIBRARY
#contains(DEFINES,QTRPT_LIBRARY) {
#    SUBDIRS += QtRPT
#}

SUBDIRS += QtRptDesigner QtRptDemo #QtRptCGI
!contains(DEFINES,NO_BARCODE) {
    prj1.file = $$PWD/zint-2.4.4/backend_qt4/Zint.pro
}

CONFIG += ordered

#OTHER_FILES = \
#    initvars.pro \
#    .qmake.conf \
#    .qmake.cache.in


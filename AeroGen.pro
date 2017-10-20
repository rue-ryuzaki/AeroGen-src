#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T16:11:50
#
#-------------------------------------------------

QT       += core gui widgets opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AeroGen
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

# Qt installation path
# ex win   INCLUDEPATH += C:/Qt
# ex linux INCLUDEPATH += /usr/include/
#INCLUDEPATH += /usr/include/ # -> for gcc6

# curl libs
# ex win   LIBS += -LC:/Qt/curl -llibcurl
# ex linux LIBS += -lcurl
# LIBS += -lcurl

LIBS += -lGLU

RC_FILE += file.rc
RESOURCES += icon.qrc

TRANSLATIONS += aerogen_en.ts
TRANSLATIONS += aerogen_ru.ts
RESOURCES += translate.qrc

SOURCES += \
    basefield.cpp \
    iniparser.cpp \
    main.cpp\
    mainwindow.cpp \
    multidla/multidla.cpp \
    multidla/cellsfield.cpp \
    multidla/mcoord.cpp \
    multidla/counter.cpp \
    multidla/logger.cpp \
    multidla/sortedvector.cpp \
    distributor/devfield.cpp \
    distributor/distributor.cpp \
    dlca/cfield.cpp \
    dlca/dlca.cpp \
    mxdla/mxfield.cpp \
    mxdla/mxdla.cpp \
    osm/ofield.cpp \
    osm/osm.cpp \
    xdla/xfield.cpp \
    xdla/xdla.cpp \
    settingsform.cpp \
    structuregl.cpp

HEADERS  += basecell.h \
    basecoord.h \
    basefield.h \
    baseformats.h \
    basegenerator.h \
    checker.h \
    figure.h \
    functions.h \
    iniparser.h \
    mainwindow.h \
    multidla/multidla.h \
    multidla/cellsfield.h \
    multidla/mcoord.h \
    multidla/counter.h \
    multidla/logger.h \
    multidla/magicdla.h \
    multidla/sortedvector.h \
    distributor/devfield.h \
    distributor/distributor.h \
    dlca/cfield.h \
    dlca/dlca.h \
    dlca/ctypes.h \
    mxdla/mxfield.h \
    mxdla/mxdla.h \
    osm/log.h \
    osm/ofield.h \
    osm/osm.h \
    osm/otypes.h \
    xdla/xfield.h \
    xdla/xdla.h \
    params.h \
    program.h \
    settingsform.h \
    structuregl.h

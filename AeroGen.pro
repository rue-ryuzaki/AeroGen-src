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

CONFIG += object_parallel_to_source

# Qwt
win32 {
#INCLUDEPATH += C:/Qt/Qwt-6.1.3/include
#LIBS += -LC:/Qt/Qwt-6.1.3/lib -lqwt
} else {
#INCLUDEPATH += /usr/local/qwt-6.1.3/include
#LIBS += -L/usr/local/qwt-6.1.3/lib -lqwt
INCLUDEPATH += /usr/include/qwt
LIBS += -lqwt-qt5
}

LIBS += -lGLU

RC_FILE += file.rc
RESOURCES += icon.qrc

TRANSLATIONS += aerogen_en.ts
TRANSLATIONS += aerogen_ru.ts
RESOURCES += translate.qrc

SOURCES += \
    basefield.cpp \
    main.cpp\
    mainwindow.cpp \
    multidla/coord.cpp \
    multidla/counter.cpp \
    multidla/field.cpp \
    multidla/logger.cpp \
    multidla/multidla.cpp \
    multidla/sortedvector.cpp \
    distributor/devfield.cpp \
    distributor/distributor.cpp \
    dlca/field.cpp \
    dlca/dlca.cpp \
    dlca2/field.cpp \
    dlca2/dlca2.cpp \
    mxdla/field.cpp \
    mxdla/mxdla.cpp \
    osm/field.cpp \
    osm/osm.cpp \
    xdla/field.cpp \
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
    flexible_field.h \
    functions.h \
    mainwindow.h \
    multidla/coord.h \
    multidla/counter.h \
    multidla/field.h \
    multidla/logger.h \
    multidla/magicdla.h \
    multidla/multidla.h \
    multidla/sortedvector.h \
    distributor/devfield.h \
    distributor/distributor.h \
    dlca/field.h \
    dlca/dlca.h \
    dlca/types.h \
    dlca2/field.h \
    dlca2/dlca2.h \
    dlca2/types.h \
    mxdla/field.h \
    mxdla/mxdla.h \
    osm/log.h \
    osm/field.h \
    osm/osm.h \
    osm/types.h \
    xdla/field.h \
    xdla/xdla.h \
    params.h \
    program.h \
    settingsform.h \
    structuregl.h

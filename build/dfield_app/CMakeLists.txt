#-------------------------------------------------
#
# Project created by QtCreator 2011-12-26T20:42:08
#
#-------------------------------------------------

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


PROJECT(dfield)

LIBS += -L../../bin/libs


LIBS += -lcommon
LIBS += -ldata

win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS
}


win32:QMAKE_CXXFLAGS += -openmp
unix:QMAKE_CXXFLAGS  += -fopenmp

unix:LIBS += -lgomp


SET(DESTDIR     ../../bin)
SET(OBJECTS_DIR ../../bin/build/dfield_app/.obj)
SET(MOC_DIR     ../../bin/build/dfield_app/.moc)
SET(RCC_DIR     ../../bin/build/dfield_app/.rcc)
SET(UI_DIR      ../../bin/build/dfield_app/.ui)

INCLUDEPATH += ../../include
INCLUDEPATH += ../../../Common/include
INCLUDEPATH += ./

SOURCES += \
    ../../src/dfield/dfield_main.cpp \

HEADERS  += \











ADD_EXECUTABLE(dfield dfield_sources)

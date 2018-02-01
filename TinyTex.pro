#-------------------------------------------------
#
# Project created by QtCreator 2013-04-06T08:57:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
VERSION = 0.2

DEFINES += KLF_SRC_BUILD

TARGET = TinyTex
TEMPLATE = app
#TEMPLATE = subdirs

#SUBDIRS += \
#        extern/klfbackend/KLFBackendIncl.pro \
#        extern/klfbackend/KLFBackend.pro \
#        src/qtranslator/QTranslator.pro \
#        src/qexprtkbackend/QExprtkBackend.pro

SOURCES += \
        src/main.cpp\
        src/mainwindow.cpp \
        src/symbollistview_w.cpp

HEADERS  += \
        src/mainwindow.h \
        src/symbollistview_w.h \
        src/global.h

FORMS    += \
        src/mainwindow.ui \
        src/symbollistview_w.ui

#SOURCES += src/*.cpp
#HEADERS += src/*.h
#FORMS   += src/*.ui



# note that in unix(linux) systems library names are case sensitive
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib
else:unix: LIBS += -L$$PWD/lib

INCLUDEPATH += $$PWD/src
#DEPENDPATH += $$PWD/src


win32:CONFIG(release, debug|release): LIBS += -lKLFBackend
else:win32:CONFIG(debug, debug|release): LIBS += -lKLFBackend
else:unix: LIBS += -lKLFBackend

INCLUDEPATH += $$PWD/extern/klfbackend
DEPENDPATH += $$PWD/extern/klfbackend


win32:CONFIG(release, debug|release): LIBS += -lqtranslator
else:win32:CONFIG(debug, debug|release): LIBS += -lqtranslator
else:unix: LIBS += -lQTranslator

INCLUDEPATH += $$PWD/src/qtranslator
DEPENDPATH += $$PWD/src/qtranslator


win32:CONFIG(release, debug|release): LIBS += -lqexprtkbackend
else:win32:CONFIG(debug, debug|release): LIBS += -lqexprtkbackend
else:unix: LIBS += -lQExprtkBackend

INCLUDEPATH += $$PWD/src/qexprtkbackend
DEPENDPATH += $$PWD/src/qexprtkbackend




include(extern/klfbackend/KLFBackend.pri)
#include(extern/klfbackend/KLFBackendHeaders.pri)
#include(extern/klfbackend/KLFBackendSources.pri)
include(src/qexprtkbackend/QExprtkBackend.pri)
#include(src/qexprtkbackend/QExprtkBackendHeaders.pri)
#include(src/qexprtkbackend/QExprtkBackendSources.pri)
include(src/qtranslator/QTranslator.pri)
#include(src/qtranslator/QTranslatorHeaders.pri)
#include(src/qtranslator/QTranslatorSources.pri)

#include(/home/muf/.strah_calc/lang/lang.pri)

DISTFILES += \
          /home/muf/.strah_calc/lang/en-GB.json \
          /home/muf/.strah_calc/lang/sl-SI.json



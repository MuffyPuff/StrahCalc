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


# note that in unix(linux) systems library names are case sensitive
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib
else:unix: LIBS += -L$$PWD/lib

INCLUDEPATH += $$PWD/src
#DEPENDPATH += $$PWD/src


win32:CONFIG(release, debug|release): LIBS += -lKLFBackend
else:win32:CONFIG(debug, debug|release): LIBS += -lKLFBackend
else:unix: LIBS += -lKLFBackend

INCLUDEPATH += $$PWD/include/klfbackend
DEPENDPATH += $$PWD/include/klfbackend


win32:CONFIG(release, debug|release): LIBS += -lqtranslator
else:win32:CONFIG(debug, debug|release): LIBS += -lqtranslator
else:unix: LIBS += -lQTranslator

INCLUDEPATH += $$PWD/include/qtranslator
DEPENDPATH += $$PWD/include/qtranslator


win32:CONFIG(release, debug|release): LIBS += -lqexprtkbackend
else:win32:CONFIG(debug, debug|release): LIBS += -lqexprtkbackend
else:unix: LIBS += -lQExprtkBackend

INCLUDEPATH += $$PWD/include/qexprtkbackend
DEPENDPATH += $$PWD/include/qexprtkbackend


include(include/klfbackend/KLFBackend.pri)
include(include/qexprtkbackend/QExprtkBackend.pri)
include(include/qtranslator/QTranslator.pri)


DISTFILES += \
          /home/muf/.strah_calc/lang/en-GB.json \
          /home/muf/.strah_calc/lang/sl-SI.json



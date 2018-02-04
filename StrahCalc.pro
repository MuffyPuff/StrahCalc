#-------------------------------------------------
#
# Project created by QtCreator 2013-04-06T08:57:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
VERSION = 0.3

DEFINES += KLF_SRC_BUILD

TARGET = StrahCalc
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

INCLUDEPATH += $$PWD/../qtranslator
DEPENDPATH += $$PWD/../qtranslator


win32:CONFIG(release, debug|release): LIBS += -lqexprtkbackend
else:win32:CONFIG(debug, debug|release): LIBS += -lqexprtkbackend
else:unix: LIBS += -lQExprtkBackend

INCLUDEPATH += $$PWD/../qexprtkbackend
DEPENDPATH += $$PWD/../qexprtkbackend


include($$PWD/include/klfbackend/KLFBackend.pri)
include($$PWD/../qexprtkbackend/QExprtkBackend.pri)
include($$PWD/../qtranslator/QTranslator.pri)

#move lang files
DISTFILES += \
          /home/muf/.strah_calc/lang/en-GB.json \
          /home/muf/.strah_calc/lang/sl-SI.json



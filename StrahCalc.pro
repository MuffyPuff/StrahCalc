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

CONFIG += c++17
CONFIG -= app_bundle

#PRECOMPILED_HEADER = src/stable.h

SOURCES += \
        src/main.cpp\
        src/mufmenu.cpp \
        src/muflatex.cpp \
        src/strahcalc.cpp \
        src/mufsymbols.cpp \
        src/muffunctions.cpp \
        src/mufsettings_w.cpp \
        src/mufexprparser.cpp \
        src/mufsymbollistview_w.cpp

HEADERS  += \
        src/global.h \
        src/mufmenu.h \
        src/muflatex.h \
        src/strahcalc.h \
        src/mufsymbols.h \
        src/muffunctions.h \
        src/mufsettings_w.h \
        src/mufexprparser.h \
        src/mufsymbollistview_w.h

FORMS    += \
        src/strahcalc.ui \
        src/mufsettings_w.ui \
        src/mufsymbollistview_w.ui


# note that in unix(linux) systems library names are case sensitive
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/bin
else:unix: LIBS += -L$$PWD/bin

INCLUDEPATH += $$PWD/src
#DEPENDPATH += $$PWD/src


win32:CONFIG(release, debug|release): LIBS += -lKLFBackend
else:win32:CONFIG(debug, debug|release): LIBS += -lKLFBackend
else:unix: LIBS += -lKLFBackend

INCLUDEPATH += $$PWD/lib/klfbackend
DEPENDPATH += $$PWD/lib/klfbackend


win32:CONFIG(release, debug|release): LIBS += -lMufTranslate
else:win32:CONFIG(debug, debug|release): LIBS += -lMufTranslate
else:unix: LIBS += -lMufTranslate

INCLUDEPATH += $$PWD/lib/MufTranslate/include
DEPENDPATH += $$PWD/lib/MufTranslate/


win32:CONFIG(release, debug|release): LIBS += -lMufExprtkBackend
else:win32:CONFIG(debug, debug|release): LIBS += -lMufExprtkBackend
else:unix: LIBS += -lMufExprtkBackend

INCLUDEPATH += $$PWD/lib/MufExprtkBackend/include
DEPENDPATH += $$PWD/lib/MufExprtkBackend/


include(lib/klfbackend/KLFBackend.pri)
include(lib/MufExprtkBackend/MufExprtkBackend.pri)
include(lib/MufTranslate/MufTranslate.pri)


DISTFILES += \
          $$PWD/functions/functions \
          $$PWD/lang/en-GB.json \
          $$PWD/lang/sl-SI.json \
          $$PWD/lang/ja-JP.json


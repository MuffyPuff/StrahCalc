#-------------------------------------------------
#
# Project created by QtCreator 2017-12-30T23:52:38
#
#-------------------------------------------------

QT       += core gui widgets

DESTDIR = ../../../lib

TARGET = QTranslator
TEMPLATE = lib

SOURCES += \
        qtranslator.cpp

HEADERS += \
        qtranslator.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

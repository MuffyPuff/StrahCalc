#-------------------------------------------------
#
# Project created by QtCreator 2017-12-30T23:52:38
#
#-------------------------------------------------

QT       += core gui widgets

DESTDIR = ../../../lib

TARGET = QExprtkBackend
TEMPLATE = lib

SOURCES += \
        qexprtkbackend.cpp \
        exprtk.hpp

HEADERS += \
        qexprtkbackend.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

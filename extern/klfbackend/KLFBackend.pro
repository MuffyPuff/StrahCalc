#-------------------------------------------------
#
# Project created by QtCreator 2013-04-10T00:00:00
#
#-------------------------------------------------

QT       += core gui widgets

# KLF backend versions
KLF_BACKEND_VERSION = 3.2.8

DEFINES +=KLF_VERSION_STRING=\\\"$$KLF_BACKEND_VERSION\\\" \
        KLF_SRC_BUILD

DESTDIR = ../build-KLFBackend

TARGET = KLFBackend
TEMPLATE = lib

SOURCES += klfbackend.cpp \
    klfdebug.cpp \
    klfblockprocess.cpp \
    klfdefs.cpp \
    klfpreviewbuilderthread.cpp

HEADERS += klfbackend.h \
    klfqt34common.h \
    klfdefs.h \
    klfdebug.h \
    klfblockprocess.h \
    klfpreviewbuilderthread.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

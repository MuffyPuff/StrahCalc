# KLF backend versions
KLF_BACKEND_VERSION = 3.2.8

DEFINES +=KLF_VERSION_STRING=\\\"$$KLF_BACKEND_VERSION\\\" \
        KLF_SRC_BUILD

OTHER_FILES += \
            ./lib/klfbackend/klfbackend.h \
            ./lib/klfbackend/klfqt34common.h \
            ./lib/klfbackend/klfdefs.h \
            ./lib/klfbackend/klfdebug.h \
            ./lib/klfbackend/klfblockprocess.h \
            ./lib/klfbackend/klfpreviewbuilderthread.h \
            ./lib/klfbackend/klfbackend.cpp \
            ./lib/klfbackend/klfdebug.cpp \
            ./lib/klfbackend/klfblockprocess.cpp \
            ./lib/klfbackend/klfdefs.cpp \
            ./lib/klfbackend/klfpreviewbuilderthread.cpp \
            ./lib/klfbackend/KLFBackend.pro

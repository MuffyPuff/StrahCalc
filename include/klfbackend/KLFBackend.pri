# KLF backend versions
KLF_BACKEND_VERSION = 3.2.8

DEFINES +=KLF_VERSION_STRING=\\\"$$KLF_BACKEND_VERSION\\\" \
        KLF_SRC_BUILD

OTHER_FILES += \
            include/klfbackend/klfbackend.h \
            include/klfbackend/klfqt34common.h \
            include/klfbackend/klfdefs.h \
            include/klfbackend/klfdebug.h \
            include/klfbackend/klfblockprocess.h \
            include/klfbackend/klfpreviewbuilderthread.h \
            include/klfbackend/klfbackend.cpp \
            include/klfbackend/klfdebug.cpp \
            include/klfbackend/klfblockprocess.cpp \
            include/klfbackend/klfdefs.cpp \
            include/klfbackend/klfpreviewbuilderthread.cpp

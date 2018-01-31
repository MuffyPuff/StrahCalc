# KLF backend versions
KLF_BACKEND_VERSION = 3.2.8

DEFINES +=KLF_VERSION_STRING=\\\"$$KLF_BACKEND_VERSION\\\" \
        KLF_SRC_BUILD

OTHER_FILES += \
            extern/klfbackend/klfbackend.h \
            extern/klfbackend/klfqt34common.h \
            extern/klfbackend/klfdefs.h \
            extern/klfbackend/klfdebug.h \
            extern/klfbackend/klfblockprocess.h \
            extern/klfbackend/klfpreviewbuilderthread.h

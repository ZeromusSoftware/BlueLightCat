TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
CXXFLAGS += -fprofile-arcs -ftest-coverage --disable-ccache
LIBS += -lgcov


include(../autotests.pri)

# Input
SOURCES += \
    tst_zlink.cpp
HEADERS += \
    tst_zlink.h

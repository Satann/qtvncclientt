#-------------------------------------------------
#
SOLUTION_DIR=$$PWD/../
QT       += widgets concurrent
CONFIG += c++11
TEMPLATE = lib
TARGET = EeoVnc
DEFINES += EEOVNC_LIBRARY
INCLUDEPATH += ../libvnc ../libvnc/common

win32{
    DESTDIR = ..\libvnc\lib
    LIBS += -L"../libvnc/lib" \
            -lvncclient \
            -lzlibstatic \
            -lws2_32
    CONFIG(debug, debug|release){ DLLDESTDIR = ../Win32/debug }
    CONFIG(release, debug|release){ DLLDESTDIR = ../Win32/release }
} macx {
    DESTDIR = $$SOLUTION_DIR/macenv
    QMAKE_LFLAGS_SONAME  = -Wl,-install_name,@rpath/
    CONFIG += unversioned_libname unversioned_soname
    LIBS += -L"$$SOLUTION_DIR/macenv/" -lvncclient -lz
}

SOURCES += \
        EeoVncWidget.cpp \
        EeoVncClient.cpp

HEADERS +=\
        EeoVncWidget.h \
        EeoVncClient.h


SOLUTION_DIR=$$PWD/../
QT       += core gui widgets concurrent

CONFIG += c++11
TARGET = VncApp
TEMPLATE = app
INCLUDEPATH += $$PWD/../EeoVnc
DEPENDPATH += $$PWD/../EeoVnc

win32{
    LIBS += -L"../libvnc/lib" -lEeoVnc
    CONFIG(debug, debug|release){DESTDIR = ../Win32/debug}
    CONFIG(release, debug|release){DESTDIR = ../Win32/release}
} macx {
    CONFIG += unversioned_libname unversioned_soname
    LIBS += -L"$$SOLUTION_DIR/macenv/" -lEeoVnc
    DESTDIR = $$SOLUTION_DIR/macenv/
    QMAKE_POST_LINK += $$quote(mkdir -p $$SOLUTION_DIR/macenv/$${TARGET}.app/Contents/Library/ && )
    QMAKE_POST_LINK += $$quote(cp $$SOLUTION_DIR/macenv/*.dylib $$SOLUTION_DIR/macenv/$${TARGET}.app/Contents/Library )
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
macx:QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Library

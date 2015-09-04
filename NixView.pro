#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T12:57:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NixView
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11


SOURCES += main.cpp\
        MainWindow.cpp \
    MainViewWidget.cpp \
    RawTreeView.cpp \
    AnotherTree.cpp

HEADERS  += MainWindow.hpp \
    MainViewWidget.hpp \
    RawTreeView.hpp \
    AnotherTree.hpp

FORMS    += MainWindow.ui \
    MainViewWidget.ui \
    RawTreeView.ui \
    AnotherTree.ui


# PROBLEM (nicht allgemein)
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../tools/nix/cmake/release/ -lnix
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../tools/nix/cmake/debug/ -lnix
else:unix: LIBS += -L$$PWD/../../../../tools/nix/cmake/ -lnix

INCLUDEPATH += $$PWD/../../../../tools/nix/cmake
DEPENDPATH += $$PWD/../../../../tools/nix/cmake
# MELBORP

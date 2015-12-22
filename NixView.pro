#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T12:57:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = NixView
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11


SOURCES += main.cpp\
        MainWindow.cpp \
    MainViewWidget.cpp \
    RawTreeView.cpp \
    InfoWidget/InfoWidget.cpp \
    InfoWidget/DescriptionPanel.cpp \
    InfoWidget/MetaDataPanel.cpp \
    InfoWidget/TagPanel.cpp \
    aboutdialog.cpp \
    plotdialog.cpp \
    qcustomplot.cpp \
    Plotter/plotter.cpp \
    nixview.cpp

HEADERS  += MainWindow.hpp \
    MainViewWidget.hpp \
    RawTreeView.hpp \
    InfoWidget/InfoWidget.hpp \
    InfoWidget/DescriptionPanel.hpp \
    InfoWidget/MetaDataPanel.hpp \
    InfoWidget/TagPanel.hpp \
    common/Common.hpp \
    aboutdialog.h \
    plotdialog.h \
    qcustomplot.h \
    Plotter/plotter.h \
    nixview.h

FORMS    += MainWindow.ui \
    MainViewWidget.ui \
    RawTreeView.ui \
    InfoWidget/InfoWidget.ui \
    InfoWidget/DescriptionPanel.ui \
    InfoWidget/MetaDataPanel.ui \
    InfoWidget/TagPanel.ui \
    aboutdialog.ui \
    plotdialog.ui


#standard windows folder?
#win32:CONFIG(release, debug|release): LIBS += /usr/local/lib/release/ -lnix
#else:win32:CONFIG(debug, debug|release): LIBS += /usr/local/lib/debug/ -lnix
#else:unix: LIBS += /usr/local/lib/ -lnix

unix: LIBS += -L/usr/local/lib/ -lnix

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

RESOURCES += \
    resources.qrc

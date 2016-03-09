#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T12:57:24
#
#-------------------------------------------------

# QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 5): QT += core gui widgets printsupport

TARGET = NixView
TEMPLATE = app

unix {
    QMAKE_CXXFLAGS+= -std=c++11
}

macx {
    CONFIG += c++11
}

SOURCES += main.cpp\
        MainWindow.cpp \
    MainViewWidget.cpp \
    views/RawTreeView.cpp \
    infowidget/InfoWidget.cpp \
    infowidget/DescriptionPanel.cpp \
    infowidget/MetaDataPanel.cpp \
    infowidget/TagPanel.cpp \
    aboutdialog.cpp \
    plotdialog.cpp \
    qcustomplot.cpp \
    nixview.cpp \
    plotwidget.cpp \
    plotter/lineplotter.cpp \
    plotter/categoryplotter.cpp\
    model/NixDataModel.cpp \
    views/ColumnView.cpp \
    filter/NixProxyModel.cpp \
    model/NixModelItem.cpp
    entitydescriptor.cpp

HEADERS  += MainWindow.hpp \
    MainViewWidget.hpp \
    views/RawTreeView.hpp \
    infowidget/InfoWidget.hpp \
    infowidget/DescriptionPanel.hpp \
    infowidget/MetaDataPanel.hpp \
    infowidget/TagPanel.hpp \
    common/Common.hpp \
    aboutdialog.h \
    plotdialog.h \
    qcustomplot.h \
    plotter/plotter.h \
    nixview.h \
    plotter/lineplotter.h \
    plotwidget.h \
    plotter/categoryplotter.h\
    model/NixDataModel.hpp \
    views/ColumnView.hpp \
    filter/NixProxyModel.hpp \
    model/NixModelItem.hpp
    entitydescriptor.h

FORMS    += MainWindow.ui \
    MainViewWidget.ui \
    views/RawTreeView.ui \
    infowidget/InfoWidget.ui \
    infowidget/DescriptionPanel.ui \
    infowidget/MetaDataPanel.ui \
    infowidget/TagPanel.ui \
    aboutdialog.ui \
    plotdialog.ui \
    plotter/lineplotter.ui \
    plotwidget.ui \
    plotter/categoryplotter.ui \
    views/ColumnView.ui

#standard windows folder?
#win32:CONFIG(release, debug|release): LIBS += /usr/local/lib/release/ -lnix
#else:win32:CONFIG(debug, debug|release): LIBS += /usr/local/lib/debug/ -lnix
#else:unix: LIBS += /usr/local/lib/ -lnix

unix: LIBS += -L/usr/local/lib/ -lnix

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

RESOURCES += \
    resources.qrc

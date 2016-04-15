#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T12:57:24
#
#-------------------------------------------------

# QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets printsupport

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
    qcustomplot.cpp \
    nixview.cpp \
    plotwidget.cpp \
    TagView.cpp \
    entitydescriptor.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/cvsexportdialog.cpp \
    dialogs/plotdialog.cpp \
    dialogs/tabledialog.cpp \
    filter/NixProxyModel.cpp \
    infowidget/InfoWidget.cpp \
    infowidget/DescriptionPanel.cpp \
    infowidget/MetaDataPanel.cpp \
    infowidget/TagPanel.cpp \
    model/NixDataModel.cpp \
    model/NixModelItem.cpp \
    options/OptionsWidget.cpp \
    options/OptionsTabWidget.cpp \
    options/TreeViewOptions.cpp \
    options/recentfileoptions.cpp\
    plotter/lineplotter.cpp \
    plotter/categoryplotter.cpp\
    plotter/colormap.cpp \
    plotter/imageplotter.cpp \
    views/ColumnView.cpp \
    views/datatable.cpp \
    views/RawTreeView.cpp \
    utils/utils.cpp

HEADERS  += MainWindow.hpp \
    MainViewWidget.hpp \
    qcustomplot.h \
    nixview.h \
    plotwidget.h \
    TagView.hpp \
    common/Common.hpp \
    dialogs/aboutdialog.h \
    dialogs/cvsexportdialog.h \
    dialogs/plotdialog.h \
    dialogs/tabledialog.hpp \
    filter/NixProxyModel.hpp \
    infowidget/InfoWidget.hpp \
    infowidget/DescriptionPanel.hpp \
    infowidget/MetaDataPanel.hpp \
    infowidget/TagPanel.hpp \
    model/NixDataModel.hpp \
    model/NixModelItem.hpp \
    entitydescriptor.h \
    options/OptionsWidget.hpp \
    options/OptionsTabWidget.hpp \
    options/TreeViewOptions.hpp\
    options/recentfileoptions.hpp \
    plotter/categoryplotter.h\
    plotter/colormap.hpp \
    plotter/imageplotter.h \
    plotter/lineplotter.h \
    plotter/plotter.h \
    views/datatable.h \
    views/RawTreeView.hpp \
    views/ColumnView.hpp \
    utils/utils.hpp

FORMS    += MainWindow.ui \
    MainViewWidget.ui \
    plotwidget.ui \
    TagView.ui \
    dialogs/aboutdialog.ui \
    dialogs/cvsexportdialog.ui \
    dialogs/plotdialog.ui \
    dialogs/tabledialog.ui \
    infowidget/InfoWidget.ui \
    infowidget/DescriptionPanel.ui \
    infowidget/MetaDataPanel.ui \
    infowidget/TagPanel.ui \
    plotter/lineplotter.ui \
    plotter/categoryplotter.ui \
    plotter/imageplotter.ui \
    options/OptionsWidget.ui \
    options/recentfileoptions.ui \
    options/TreeViewOptions.ui \
    views/ColumnView.ui \
    views/datatable.ui \
    views/RawTreeView.ui

#standard windows folder?
#win32:CONFIG(release, debug|release): LIBS += /usr/local/lib/release/ -lnix
#else:win32:CONFIG(debug, debug|release): LIBS += /usr/local/lib/debug/ -lnix
#else:unix: LIBS += /usr/local/lib/ -lnix

unix: LIBS += -L/usr/local/lib/ -lnix

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

RESOURCES += \
    resources.qrc

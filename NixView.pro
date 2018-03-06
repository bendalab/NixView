#-------------------------------------------------
#
# Project created by QtCreator 2015-07-20T12:57:24
#
#-------------------------------------------------

# QT       += core gui widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets printsupport sql

TARGET = NixView
TEMPLATE = app

unix {
    QMAKE_CXXFLAGS+= -std=c++11
}

macx {
    CONFIG += c++11
    QMAKE_CXXFLAGS += -std=c++11
}

SOURCES += main.cpp\
    MainWindow.cpp \
    nixview.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/csvexportdialog.cpp \
    dialogs/plotdialog.cpp \
    dialogs/tabledialog.cpp \
    filter/NixProxyModel.cpp \
    infowidget/InfoWidget.cpp \
    infowidget/DescriptionPanel.cpp \
    infowidget/MetaDataPanel.cpp \
    infowidget/TagPanel.cpp \
    model/nixarraytablemodel.cpp \
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
    plotter/plotwidget.cpp \
    plotter/qcustomplot.cpp \
    views/ColumnView.cpp \
    views/datatable.cpp \
    views/MainViewWidget.cpp \
    views/RawTreeView.cpp \
    views/TagView.cpp \
    utils/entitydescriptor.cpp \
    utils/utils.cpp \
    model/nixtreemodelitem.cpp \
    model/nixtreemodel.cpp \
    views/lazyloadview.cpp \
    dialogs/optionsdialog.cpp \
    dialogs/filepropertiesdialog.cpp \
    model/nixmetadatatreemodel.cpp \
    searchform.cpp \
    db/projectmanager.cpp \
    views/projectnavigator.cpp \
    db/projectindex.cpp \
    plotter/eventplotter.cpp \
    utils/tagcontainer.cpp


HEADERS  += MainWindow.hpp \
    nixview.h \
    common/Common.hpp \
    dialogs/aboutdialog.h \
    dialogs/csvexportdialog.h \
    dialogs/plotdialog.h \
    dialogs/tabledialog.hpp \
    filter/NixProxyModel.hpp \
    infowidget/InfoWidget.hpp \
    infowidget/DescriptionPanel.hpp \
    infowidget/MetaDataPanel.hpp \
    infowidget/TagPanel.hpp \
    model/nixarraytablemodel.h \
    model/NixDataModel.hpp \
    model/NixModelItem.hpp \
    utils/entitydescriptor.h \
    options/OptionsWidget.hpp \
    options/OptionsTabWidget.hpp \
    options/TreeViewOptions.hpp\
    options/recentfileoptions.hpp \
    plotter/categoryplotter.h\
    plotter/colormap.hpp \
    plotter/imageplotter.h \
    plotter/lineplotter.h \
    plotter/plotter.h \
    plotter/plotwidget.h \
    plotter/qcustomplot.h \
    views/ColumnView.hpp \
    views/datatable.h \
    views/MainViewWidget.hpp \
    views/RawTreeView.hpp \
    views/TagView.hpp \
    utils/entitydescriptor.h \
    utils/utils.hpp \
    model/nixtreemodelitem.h \
    model/nixtreemodel.h \
    views/lazyloadview.h \
    dialogs/optionsdialog.h \
    dialogs/filepropertiesdialog.hpp \
    model/nixmetadatatreemodel.h \
    searchform.h \
    db/projectmanager.hpp \
    views/projectnavigator.hpp \
    db/projectindex.hpp \
    plotter/eventplotter.h \
    utils/tagcontainer.h


FORMS    += MainWindow.ui \
    dialogs/aboutdialog.ui \
    dialogs/csvexportdialog.ui \
    dialogs/plotdialog.ui \
    dialogs/tabledialog.ui \
    infowidget/InfoWidget.ui \
    infowidget/DescriptionPanel.ui \
    infowidget/MetaDataPanel.ui \
    infowidget/TagPanel.ui \
    plotter/lineplotter.ui \
    plotter/categoryplotter.ui \
    plotter/imageplotter.ui \
    plotter/plotwidget.ui \
    options/OptionsWidget.ui \
    options/recentfileoptions.ui \
    options/TreeViewOptions.ui \
    views/ColumnView.ui \
    views/datatable.ui \
    views/MainViewWidget.ui \
    views/RawTreeView.ui \
    views/TagView.ui \
    views/lazyloadview.ui \
    dialogs/optionsdialog.ui \
    dialogs/filepropertiesdialog.ui \
    searchform.ui \
    views/projectnavigator.ui \
    plotter/eventplotter.ui

#standard windows folder?
#win32:CONFIG(release, debug|release): LIBS += /usr/local/lib/release/ -lnix
#else:win32:CONFIG(debug, debug|release): LIBS += /usr/local/lib/debug/ -lnix
#else:unix: LIBS += /usr/local/lib/ -lnix

unix: LIBS += -L/usr/local/lib/ -lnix \
                -lboost_filesystem\
                -lboost_system

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

RESOURCES += \
    resources.qrc

DISTFILES += \
    images/info.png

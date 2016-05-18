#include "lazyloadview.h"
#include "ui_lazyloadview.h"
#include "model/nixtreemodel.h"
#include <QSettings>
#include "common/Common.hpp"
#include <iostream>
#include <QMenu>


LazyLoadView::LazyLoadView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LazyLoadView)
{
    ui->setupUi(this);
    QObject::connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_requested(QPoint)));
}

LazyLoadView::~LazyLoadView() {
    delete ui;
}


QTreeView* LazyLoadView::getTreeView() {
    return ui->treeView;
}


void LazyLoadView::set_columns() {
    QSettings *settings = new QSettings;
    settings->beginGroup(MAIN_TREE_VIEW);
    for (QString s : NixTreeModelItem::columns) {
        set_column_state(s, settings->value(s, QVariant(true)).toBool());
    }
    settings->endGroup();
    delete settings;
}


void LazyLoadView::set_column_state(QString column, bool visible) {
    NixTreeModel *model = static_cast<NixTreeModel*>(ui->treeView->model());
    if (model == nullptr)
        return;
    for (int i = 0; i < model->columnCount(); i++) {
        if (model->headerData(i, Qt::Horizontal).toString() == column) {
            ui->treeView->setColumnHidden(i, !visible);
        }
    }
    for (int i = 0; i < model->columnCount(); i++) {
        ui->treeView->resizeColumnToContents(i);
    }
}


void LazyLoadView::expandAll() {
    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();
    ui->treeView->expandAll();
    this->setCursor(Qt::ArrowCursor);
}


void LazyLoadView::context_menu_requested(QPoint pos) {
    QMenu *menu = new QMenu(this);
    menu->addAction("collapse all", ui->treeView, SLOT(collapseAll()));
    menu->addAction("expand all", this, SLOT(expandAll()));
    //menu->addAction("collapse all", this);
    //menu->addAction("expand all", this);
    menu->exec(ui->treeView->mapToGlobal(pos));
}

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
    QObject::connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

LazyLoadView::~LazyLoadView() {
    delete ui;
}


QTreeView* LazyLoadView::getTreeView() {
    return ui->treeView;
}


void LazyLoadView::setColumns() {
    QSettings *settings = new QSettings;
    settings->beginGroup(MAIN_TREE_VIEW);
    for (QString s : NixTreeModelItem::columns) {
        setColumnState(s, settings->value(s, QVariant(true)).toBool());
    }
    settings->endGroup();
    delete settings;
}


void LazyLoadView::setColumnState(QString column, bool visible) {
    NixTreeModel *model = static_cast<NixTreeModel*>(ui->treeView->model());
    if (model == nullptr)
        return;

    for (int i = 0; i < NixTreeModelItem::columns.size(); i++) {
        if (model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() == column) {
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


void LazyLoadView::contextMenuRequest(QPoint pos) {
    QMenu *menu = new QMenu(this);
    menu->addAction("collapse all", ui->treeView, SLOT(collapseAll()));
    menu->addAction("expand all", this, SLOT(expandAll()));
    menu->exec(ui->treeView->mapToGlobal(pos));
}


void LazyLoadView::resizeRequest() {
    NixTreeModel *model = static_cast<NixTreeModel*>(ui->treeView->model());
    for (int c = 0; c<model->columnCount(); c++)
        ui->treeView->resizeColumnToContents(c);
}

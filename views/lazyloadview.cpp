#include "lazyloadview.h"
#include "ui_lazyloadview.h"
#include "model/nixtreemodel.h"
#include <QSettings>
#include "common/Common.hpp"
#include <iostream>


LazyLoadView::LazyLoadView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LazyLoadView)
{
    ui->setupUi(this);
}

LazyLoadView::~LazyLoadView() {
    delete ui;
}


QTreeView* LazyLoadView::getTreeView() {
    return ui->treeView;
}


void LazyLoadView::set_columns() {
    QSettings *settings = new QSettings;
    settings->beginGroup("TreeColumns");
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

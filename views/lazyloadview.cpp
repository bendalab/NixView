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


void LazyLoadView::hide_columns() {
    QSettings *settings = new QSettings();
    settings->beginGroup(S_RAWTREEVIEW);
    settings->beginGroup(S_COLUMNSHIDDEN);
    NixTreeModel *model = static_cast<NixTreeModel*>(ui->treeView->model());
    if (model == nullptr) {
        return;
    }
    QVector<QString> headers;
    for (int i = 0; i < model->columnCount(); i++) {
        headers.push_back(model->headerData(i, Qt::Horizontal).toString());
    }

    for (int i = 0; i < headers.size(); ++i) {
        bool hidden = settings->value(headers[i]).toBool();
        ui->treeView->setColumnHidden(i, hidden);
    }
    settings->endGroup();
    settings->endGroup();
    delete settings;
    for (int i = 0; i < model->columnCount(); i++) {
        ui->treeView->resizeColumnToContents(i);
    }
}

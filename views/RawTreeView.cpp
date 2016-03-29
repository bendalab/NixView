#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"
#include <qdebug.h>

RawTreeView::RawTreeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    filter_mode = 0;

    nix_proxy_model = nullptr;

    default_hidden_columns = {MODEL_HEADER_ID,
                              MODEL_HEADER_CREATEDAT,
                              MODEL_HEADER_UPDATEDAT,
                              MODEL_HEADER_ROOTCHILDLINK};

    settings = new QSettings();
}

RawTreeView::RawTreeView(NixProxyModel* _nix_proxy_model, QWidget *parent) :
    RawTreeView(parent)
{
    set_proxy_model(_nix_proxy_model);
}

void RawTreeView::set_proxy_model(NixProxyModel *proxy_model)
{
    nix_proxy_model = proxy_model;

    ui->treeView->setModel(nix_proxy_model);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    hide_columns();
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);

}

// slots
void RawTreeView::resize_to_content(QModelIndex)
{
    for (int c = 0; c<nix_proxy_model->columnCount();c++)
        ui->treeView->resizeColumnToContents(c);
}

void RawTreeView::hide_columns()
{
    settings->beginGroup(S_RAWTREEVIEW);
    settings->beginGroup(S_COLUMNS);
    RowStrings headers = static_cast<NixDataModel*>(nix_proxy_model->sourceModel())->get_headers();
    if (!(nix_proxy_model == nullptr))
        for (int i = 0; i < headers.size(); ++i)
        {
            bool hidden = settings->value(headers[i], default_hidden_columns.contains(headers[i]) ? true : false).toBool();
            ui->treeView->setColumnHidden(i, hidden);
        }
    settings->endGroup();
    settings->endGroup();
}

// getter
QTreeView* RawTreeView::get_tree_view()
{
    return ui->treeView;
}


// destructor
RawTreeView::~RawTreeView()
{
    delete ui;
}

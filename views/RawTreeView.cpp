#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"

RawTreeView::RawTreeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    filter_mode = 0;

    nix_proxy_model = nullptr;
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
    fixed_hidden_columns = {10};
    set_hidden_columns({5, 6, 7});
    for (int entry : hidden_columns)
        ui->treeView->setColumnHidden(entry, true);
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);

}

// slots
void RawTreeView::resize_to_content(QModelIndex)
{
    for (int c = 0; c<nix_proxy_model->columnCount();c++)
        ui->treeView->resizeColumnToContents(c);
}

void RawTreeView::set_hidden_columns(std::vector<int> cols)
{
    for(int c : fixed_hidden_columns)
        cols.push_back(c);
    hidden_columns = cols;
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

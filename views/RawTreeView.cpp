#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"
#include <qdebug.h>

QStringList RawTreeView::DEFAULT_HIDDEN_COLUMNS = {MODEL_HEADER_ID,
                                                   MODEL_HEADER_CREATEDAT,
                                                   MODEL_HEADER_UPDATEDAT,
                                                   MODEL_HEADER_ROOTCHILDLINK};

RawTreeView::RawTreeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    filter_mode = 0;
    nix_proxy_model = nullptr;
    current_depth = 0;
    settings = new QSettings();
}


void RawTreeView::set_proxy_model(NixProxyModel *proxy_model)
{
    nix_proxy_model = proxy_model;

    ui->treeView->setModel(nix_proxy_model);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    hide_columns();
    ui->treeView->sortByColumn(0, Qt::AscendingOrder);

}

int RawTreeView::calc_depth_from_qml(QModelIndex qml)
{
    QModelIndex index = qml;
    int depth = 0;
    while (index.parent().isValid())
    {
        index = index.parent();
        ++depth;
    }
    return depth;
}

// slots
void RawTreeView::resize_to_content(QModelIndex)
{
    for (int c = 0; c<nix_proxy_model->columnCount();c++)
        ui->treeView->resizeColumnToContents(c);
}

void RawTreeView::expand_collapse(QString)
{
    ui->treeView->expandToDepth(current_depth);
}

void RawTreeView::expand_collapse(bool)
{
    ui->treeView->expandToDepth(current_depth);
}

void RawTreeView::set_current_depth_expanded(QModelIndex qml)
{
    current_depth = calc_depth_from_qml(qml);
}

void RawTreeView::set_current_depth_collapsed(QModelIndex qml)
{
    current_depth = calc_depth_from_qml(qml) - 1;
}

void RawTreeView::hide_columns()
{
    settings->beginGroup(S_RAWTREEVIEW);
    settings->beginGroup(S_COLUMNSHIDDEN);
    RowStrings headers = static_cast<NixDataModel*>(nix_proxy_model->sourceModel())->get_headers();
    if (!(nix_proxy_model == nullptr))
        for (int i = 0; i < headers.size(); ++i)
        {
            bool hidden = settings->value(headers[i], RawTreeView::DEFAULT_HIDDEN_COLUMNS.contains(headers[i]) ? true : false).toBool();
            ui->treeView->setColumnHidden(i, hidden);
        }
    settings->endGroup();
    settings->endGroup();

   resize_to_content(QModelIndex());
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

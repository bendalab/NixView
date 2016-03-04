#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"


RawTreeView::RawTreeView(NixDataModel* _nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    filter_mode = 0;
    nix_model = _nix_model;
    ui->treeView->setModel(nix_model);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    hidden_columns = {5, 6, 7, 8, 9};
    for (int entry : hidden_columns)
        ui->treeView->setColumnHidden(entry, true);
}

// slots
void RawTreeView::resize_to_content(QModelIndex qml) {
    for (int c = 0; c<nix_model->columnCount();c++)
        ui->treeView->resizeColumnToContents(c);
}

//void RawTreeView::expand()
//{
//    if (filter_mode == 0 || filter_mode == 2)
//        ui->treeWidget->expandToDepth(0);
//    else
//        ui->treeWidget->expandToDepth(1);
//}

void RawTreeView::current_changed(QModelIndex qml_new, QModelIndex qml_old)
{
//    qDebug() << qml_old;
//    qDebug() << qml_new;
}

void RawTreeView::selection_changed(QItemSelection qml_new, QItemSelection qml_old)
{
    //qDebug() << "test";
}

// getter
const QTreeView* RawTreeView::get_tree_view()
{
    return ui->treeView;
}
//const QComboBox* RawTreeView::get_filter_combo_box()
//{
//    return ui->comboBox;
//}


// destructor
RawTreeView::~RawTreeView()
{
    delete ui;
}

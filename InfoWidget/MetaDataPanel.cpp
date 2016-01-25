#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"
#include "common/Common.hpp"
#include <ostream>

MetaDataPanel::MetaDataPanel(NixDataModel *_nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);

    proxy_model = new QSortFilterProxyModel(this);
    proxy_model->setSourceModel(_nix_model);
    ui->treeView->setModel(proxy_model);
}



void MetaDataPanel::clear_metadata_panel()
{
    // ui->treeWidget->clear();
}

void MetaDataPanel::resize_to_content(QModelIndex qml)
{
//    for (int c = 0; c<ui->treeWidget->columnCount();c++)
//        ui->treeWidget->resizeColumnToContents(c);
}

//getter
const QTreeView* MetaDataPanel::get_tree_view()
{
    return ui->treeView;
}

MetaDataPanel::~MetaDataPanel()
{
    delete ui;
}

#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"
#include "common/Common.hpp"
#include "model/nixtreemodelitem.h"
#include <ostream>

MetaDataPanel::MetaDataPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);
}


void MetaDataPanel::setDataModel(NixTreeModel *_nix_model) {
    proxy_model = new QSortFilterProxyModel(this);
    proxy_model->setSourceModel(_nix_model);
    ui->treeView->setModel(proxy_model);
    std::vector<int> hidden_columns = {2,3,4,5,6,7,10};
    for(int c : hidden_columns)
        ui->treeView->setColumnHidden(c, true);
    set_proxy_model();
}


void MetaDataPanel::set_proxy_model() {
    clear_metadata_panel();
    //proxy_model->set_filter_mode(3);
    //proxy_model->set_metadata_only_mode(true);
}


void MetaDataPanel::update_metadata_panel(QModelIndex qml) {
    //QModelIndex src_qml = proxy_model->mapToSource(qml);
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    /*
    if(qml.isValid() && item->entity_can_have_metadata())
    {
        nix::Section metadata = item->get_entity_metadata();
        if(metadata)
        {
            proxy_model->set_block_mode(false);
            proxy_model->set_fine_filter(QString::fromStdString(metadata.id()));
            ui->treeView->expandAll();
        }
        else
            clear_metadata_panel();
    }
    else
        clear_metadata_panel();
    */
}

void MetaDataPanel::clear_metadata_panel()
{
    //proxy_model->set_block_mode(true);
    //proxy_model->refresh();
}

void MetaDataPanel::resize_to_content(QModelIndex)
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

#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"
#include "common/Common.hpp"
#include <ostream>

MetaDataPanel::MetaDataPanel(NixDataModel *_nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);

    proxy_model = new NixProxyModel(this);
    proxy_model->setSourceModel(_nix_model);
    ui->treeView->setModel(proxy_model);
    set_proxy_model();
}

void MetaDataPanel::set_proxy_model()
{
//    proxy_model->setFilterRegExp(QRegExp("Metadata", Qt::CaseInsensitive, QRegExp::FixedString));
    proxy_model->setFilterRegExp("MetaData");
    proxy_model->set_filter_mode(1);
}

void MetaDataPanel::update_metadata_panel(QModelIndex qml)
{
    NixModelItem *item = static_cast<NixModelItem*>(static_cast<NixDataModel*>(proxy_model->sourceModel())->itemFromIndex(qml));
    if(item->entity_can_have_metadata())
    {
        nix::Section metadata = item->get_entity_metadata();
        qDebug() << QString::fromStdString(metadata.name());
        proxy_model->setFilterRegExp(QString::fromStdString(metadata.name()));
    }
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

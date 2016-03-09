#include "InfoWidget.hpp"
#include "ui_InfoWidget.h"
#include <sstream>
#include "common/Common.hpp"
#include "MainViewWidget.hpp"
#include <QDebug>

InfoWidget::InfoWidget(NixDataModel *_nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoWidget)
{
    ui->setupUi(this);

    mp = new MetaDataPanel(_nix_model, this);
    ui->verticalLayout_page_meta->addWidget(mp);

    tp = new TagPanel(this);
    ui->verticalLayout_page_tag->addWidget(tp);

    dp = new DescriptionPanel(this);
    ui->verticalLayout_page_info->addWidget(dp);

    ui->tabWidget->setCurrentIndex(0);

    connect_widgets();
}

void InfoWidget::update_info_widget(QModelIndex qml)
{
    mp->update_metadata_panel(qml);

    NixDataModel *current_model = MainViewWidget::get_current_model();
    NixModelItem *model_item = static_cast<NixModelItem*>(current_model->itemFromIndex(qml));

    tp->update_tag_panel(qml);
    dp->update_description_panel(qml);
    if(strcmp(model_item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0 ||
            strcmp(model_item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0)
        ui->tabWidget->setCurrentIndex(1);
    else
        ui->tabWidget->setCurrentIndex(0);
}

void InfoWidget::connect_widgets()
{
//    QObject::connect(mp->get_tree_widget(), SIGNAL(expanded(QModelIndex)), mp, SLOT(resize_to_content(QModelIndex)));
//    QObject::connect(mp->get_tree_widget(), SIGNAL(collapsed(QModelIndex)), mp, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(tp->get_tag_table(), SIGNAL(currentCellChanged(int,int,int,int)), tp, SLOT(tag_item_requested(int, int, int, int)));

    QObject::connect(tp->get_reference_tree(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                     tp, SLOT(currentItemChanged_reference_helper(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(tp->get_feature_tree(), SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                     tp, SLOT(currentItemChanged_feature_helper(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(tp->get_tag_table(), SIGNAL(currentCellChanged(int,int,int,int)),
                     tp, SLOT(tag_item_requested(int,int,int,int)));
}

// getter
const MetaDataPanel* InfoWidget::get_metadata_panel()
{
    return mp;
}

InfoWidget::~InfoWidget()
{
    delete ui;
}



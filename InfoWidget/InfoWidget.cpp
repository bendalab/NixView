#include "InfoWidget.hpp"
#include "ui_InfoWidget.h"
#include <sstream>
#include "common/Common.hpp"

InfoWidget::InfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoWidget)
{
    ui->setupUi(this);

    dp = new DescriptionPanel();
    ui->verticalLayout->addWidget(dp);

    mp = new MetaDataPanel();
    ui->verticalLayout->addWidget(mp);

    tp = new TagPanel();
    ui->verticalLayout->addWidget(tp);

    connect_widgets();
}

void InfoWidget::update_info_widget(QVariant v)
{
    dp->update_description_panel(v);
    mp->update_metadata_panel(v);
    tp->update_tag_panel(v);
}

void InfoWidget::update_info_widget()
{
    dp->clear_description_panel();
    mp->clear_metadata_panel();
}

void InfoWidget::connect_widgets()
{
    QObject::connect(mp->get_tree_widget(), SIGNAL(expanded(QModelIndex)), mp, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(mp->get_tree_widget(), SIGNAL(collapsed(QModelIndex)), mp, SLOT(resize_to_content(QModelIndex)));
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



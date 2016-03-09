#include "DescriptionPanel.hpp"
#include "ui_DescriptionPanel.h"
#include <common/Common.hpp>
#include <time.h>
#include <boost/algorithm/string.hpp>
#include "MainViewWidget.hpp"

DescriptionPanel::DescriptionPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptionPanel)
{
    ui->setupUi(this);
}

void DescriptionPanel::update_description_panel(QModelIndex qml)
{
    NixModelItem* item = MainViewWidget::get_current_model()->get_item_from_qml(qml);

    if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_BLOCK) == 0)
        update(item->get_nix_entity<nix::Block>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0)
            update(item->get_nix_entity<nix::DataArray>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0)
            update(item->get_nix_entity<nix::MultiTag>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0)
            update(item->get_nix_entity<nix::Tag>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_SECTION) == 0)
            update(item->get_nix_entity<nix::Section>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_SOURCE) == 0)
            update(item->get_nix_entity<nix::Source>());
    else if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_PROPERTY) == 0)
            update_typeless(item->get_nix_entity<nix::Property>());
    else
        clear_description_panel();
}

template<typename T>
void DescriptionPanel::update(T arg)
{
    time_t rawtime_cr = arg.createdAt();
    struct tm *info_cr;
    info_cr = localtime( &rawtime_cr );

    time_t rawtime_up = arg.updatedAt();
    struct tm *info_up;
    info_up = localtime( &rawtime_up );

    std::stringstream ss;
    ss << "id: " << arg.id() << "\n"
       << "type: " << arg.type() << "\n"
       << "name: " << arg.name() << "\n"
       << "created at: " << boost::algorithm::trim_right_copy(std::string(asctime(info_cr))) << "\n"
       << "updated at: " << boost::algorithm::trim_right_copy(std::string(asctime(info_up))) << "\n"
       << "description: ";
    if (arg.definition().is_initialized())
        ss << arg.definition().get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);
}

template<typename T>
void DescriptionPanel::update_typeless(T arg)
{
    time_t rawtime_cr = arg.createdAt();
    struct tm *info_cr;
    info_cr = localtime( &rawtime_cr );

    time_t rawtime_up = arg.updatedAt();
    struct tm *info_up;
    info_up = localtime( &rawtime_up );

    std::stringstream ss;
    ss << "id: " << arg.id() << "\n"
       << "name: " << arg.name() << "\n"
       << "created at: " << boost::algorithm::trim_right_copy(std::string(asctime(info_cr))) << "\n"
       << "updated at: " << boost::algorithm::trim_right_copy(std::string(asctime(info_up))) << "\n"
       << "description: ";
    if (arg.definition().is_initialized())
        ss << arg.definition().get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);
}

void DescriptionPanel::clear_description_panel()
{
    ui->info_text_edit->setText("");
}


DescriptionPanel::~DescriptionPanel()
{
    delete ui;
}

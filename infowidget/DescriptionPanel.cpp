#include "DescriptionPanel.hpp"
#include "ui_DescriptionPanel.h"
#include <common/Common.hpp>
#include <time.h>
#include <boost/algorithm/string.hpp>
#include "views/MainViewWidget.hpp"

DescriptionPanel::DescriptionPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptionPanel)
{
    ui->setupUi(this);
}

void DescriptionPanel::update_description_panel(QModelIndex qml) {
    if(!qml.isValid()) {
        clear_description_panel();
        return;
    }
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    NixType type = item->nixType();
    switch (type) {
        case (NixType::NIX_BLOCK):
            update(item->itemData().value<nix::Block>());
            break;
        case (NixType::NIX_DATA_ARRAY):
            update(item->itemData().value<nix::DataArray>());
            break;
        case (NixType::NIX_MTAG):
            update(item->itemData().value<nix::MultiTag>());
            break;
        case (NixType::NIX_TAG):
            update(item->itemData().value<nix::Tag>());
            break;
        case (NixType::NIX_SOURCE):
            update(item->itemData().value<nix::Source>());
            break;
        case (NixType::NIX_PROPERTY):
            update(item->itemData().value<nix::Property>());
            break;
        case (NixType::NIX_SECTION):
            update(item->itemData().value<nix::Section>());
            break;
        case (NixType::NIX_FEAT):
            update(item->itemData().value<nix::Feature>());
            break;
        case (NixType::NIX_GROUP):
            update(item->itemData().value<nix::Group>());
            break;
        default:
            clear_description_panel();
    }
}


template<typename T>
void DescriptionPanel::update(T arg) {
    ui->info_text_edit->setText(QString::fromStdString(EntityDescriptor::describe(arg)));
}


template<typename T>
void DescriptionPanel::update_typeless(T arg) {
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

void DescriptionPanel::clear_description_panel() {
    ui->info_text_edit->setText("");
}


DescriptionPanel::~DescriptionPanel() {
    delete ui;
}

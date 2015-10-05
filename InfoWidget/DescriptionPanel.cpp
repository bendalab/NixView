#include "DescriptionPanel.hpp"
#include "ui_DescriptionPanel.h"

DescriptionPanel::DescriptionPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptionPanel)
{
    ui->setupUi(this);
}

void DescriptionPanel::update_description_panel(std::string id, std::string type, std::string name, boost::optional<std::basic_string<char>> description)
{
    //concat strings
    std::stringstream ss;
    ss << "id: " << id << "\n"
       << "type: " << type << "\n"
       << "name: " << name << "\n"
       << "description: ";
    if (description.is_initialized())
        ss << description.get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);
}

void DescriptionPanel::update_description_panel(std::string id, std::string name, boost::optional<std::basic_string<char>> description)
{
    //concat strings
    std::stringstream ss;
    ss << "id: " << id << "\n"
       << "name: " << name << "\n"
       << "description: ";
    if (description.is_initialized())
        ss << description.get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);
}

void DescriptionPanel::update_description_panel(QVariant v)
{
    std::cout << "test" << std::endl;
}



DescriptionPanel::~DescriptionPanel()
{
    delete ui;
}

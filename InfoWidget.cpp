#include "InfoWidget.hpp"
#include "ui_InfoWidget.h"
#include <sstream>

InfoWidget::InfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoWidget)
{
    ui->setupUi(this);
    is_active = false;
}

void InfoWidget::update_info_widget(std::string id, std::string type, std::string name, boost::optional<std::basic_string<char>> description)
{
    if (!is_active)
    {
        emit add_info_widget();
        is_active = true;
    }

    ui->test_label->setText(QString::fromStdString(std::string("Selected item: ") + name));

    //concat strings
    std::stringstream ss;
    ss << "id: " << id << "\n"
       << "type: " << type << "\n"
       << "name: " << name << "\n"
       << "description: " << description;
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);
}

InfoWidget::~InfoWidget()
{
    delete ui;
}

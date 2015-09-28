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

void InfoWidget::update_info_widget(std::string id, std::string type, std::string name, boost::optional<std::basic_string<char>> description, nix::Section meta_section)
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
       << "description: ";
    if (description.is_initialized())
        ss << description.get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);

    update_meta_info(meta_section);
}

void InfoWidget::update_info_widget(std::string id, std::string name, boost::optional<std::basic_string<char>> description, nix::Section meta_section)
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
       << "name: " << name << "\n"
       << "description: ";
    if (description.is_initialized())
        ss << description.get();
    else
        ss << "-";
    QString info_string = QString::fromStdString(ss.str());
    ui->info_text_edit->setText(info_string);

    update_meta_info(meta_section);
}

void InfoWidget::update_meta_info(nix::Section meta_section)
{
    ui->treeWidget->clear();

    if(meta_section)
    {
        QTreeWidgetItem* branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(meta_section.name())));

        for (nix::Section s : meta_section.sections())
        {
            QTreeWidgetItem* tree_item = new QTreeWidgetItem(branch, QStringList(QString::fromStdString(s.name())));
            add_children_to_item(tree_item, s);
        }

        ui->treeWidget->expandItem(branch);
        ui->treeWidget->resizeColumnToContents(0);
    }
}

void InfoWidget::add_children_to_item(QTreeWidgetItem* item, nix::Section section)
{
    for  (auto s : section.sections())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(s.name())));
        add_children_to_item(child_item, s);
    }

    for (nix::Property p : section.properties())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(p.name())));
        child_item->setText(1, QString::fromStdString("Metadata"));
        child_item->setText(2, QString::fromStdString(nix::data_type_to_string(p.dataType())));
    }
}

void InfoWidget::resize_to_content(QModelIndex qml)
{
    for (int c = 0; c<ui->treeWidget->columnCount();c++)
        ui->treeWidget->resizeColumnToContents(c);
}

InfoWidget::~InfoWidget()
{
    delete ui;
}

//getter
const QTreeWidget* InfoWidget::get_tree_widget()
{
    return ui->treeWidget;
}

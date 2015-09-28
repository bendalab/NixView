#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"

MetaDataPanel::MetaDataPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);
}

void MetaDataPanel::update_metadata_panel(nix::Section meta_section)
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
        add_properties_to_item(branch, meta_section);

        ui->treeWidget->expandItem(branch);
        ui->treeWidget->resizeColumnToContents(0);
    }
}

void MetaDataPanel::add_children_to_item(QTreeWidgetItem* item, nix::Section section)
{
    for  (auto s : section.sections())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(s.name())));
        add_children_to_item(child_item, s);
    }

    add_properties_to_item(item, section);
}

void MetaDataPanel::add_properties_to_item(QTreeWidgetItem* item, nix::Section section)
{
    for (nix::Property p : section.properties())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(p.name())));
        child_item->setText(1, QString::fromStdString("Metadata"));
        child_item->setText(2, QString::fromStdString(nix::data_type_to_string(p.dataType())));
    }
}

void MetaDataPanel::resize_to_content(QModelIndex qml)
{
    for (int c = 0; c<ui->treeWidget->columnCount();c++)
        ui->treeWidget->resizeColumnToContents(c);
}

//getter
const QTreeWidget* MetaDataPanel::get_tree_widget()
{
    return ui->treeWidget;
}

MetaDataPanel::~MetaDataPanel()
{
    delete ui;
}

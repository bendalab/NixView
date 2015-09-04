#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>

RawTreeView::RawTreeView(nix::File nix_file, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);

    QList<QTreeWidgetItem*> top_level_items;

    for (nix::Block b : nix_file.blocks())
    {
        QTreeWidgetItem* tree_item = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(b.name())));
        for (nix::DataArray da  : b.dataArrays())
        {
            QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(da.name())));
            child_item->setText(1, QString::fromStdString("Data Array"));
            child_item->setText(2, QString::fromStdString(nix::data_type_to_string(da.dataType())));
        }
    }

    for (nix::Section s : nix_file.sections())
    {
        QTreeWidgetItem* tree_item = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(s.name())));
        add_children_to_item(tree_item, s);
    }

    ui->treeWidget->addTopLevelItems(top_level_items);
    ui->treeWidget->expandAll();
    for (int c = 0; c<ui->treeWidget->columnCount();c++)
        ui->treeWidget->resizeColumnToContents(c);
}

void RawTreeView::add_children_to_item(QTreeWidgetItem* item, nix::Section section)
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

RawTreeView::~RawTreeView()
{
    delete ui;
}

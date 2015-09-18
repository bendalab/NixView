#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>

RawTreeView::RawTreeView(nix::File _nix_file, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    nix_file =  _nix_file;
    init_tree_widget();

    QObject::connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(entry_double_clicked(QTreeWidgetItem*,int)));
}

void RawTreeView::init_tree_widget()
{
    QTreeWidgetItem* data_branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString("Data")));

    for (nix::Block b : nix_file.blocks())
    {
        QTreeWidgetItem* tree_item = new QTreeWidgetItem(data_branch, QStringList(QString::fromStdString(b.name())));
        for (nix::DataArray da  : b.dataArrays())
        {
            QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(da.name())));
            child_item->setText(1, QString::fromStdString("Data Array"));
            child_item->setText(2, QString::fromStdString(nix::data_type_to_string(da.dataType())));
        }

        for (nix::Tag t : b.tags())
        {
            QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(t.name())));
            child_item->setText(1, QString::fromStdString("Tag"));
        }

        for (nix::MultiTag m : b.multiTags())
        {
            QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(m.name())));
            child_item->setText(1, QString::fromStdString("MultiTag"));
        }
    }

    QTreeWidgetItem* metadata_branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString("Metadata")));

    for (nix::Section s : nix_file.sections())
    {
        QTreeWidgetItem* tree_item = new QTreeWidgetItem(metadata_branch, QStringList(QString::fromStdString(s.name())));
        add_children_to_item(tree_item, s);
    }

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

void RawTreeView::entry_double_clicked(QTreeWidgetItem* item, int column)
{
    if (item->childCount() == 0)
        std::cout << item->text(0).toStdString() << std::endl;
}

RawTreeView::~RawTreeView()
{
    delete ui;
}

#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

RawTreeView::RawTreeView(nix::File _nix_file, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    nix_file =  _nix_file;
    init_tree_widget();
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
            std::stringstream s;
            s << da.dataExtent();
            std::string shape = s.str();
            boost::algorithm::trim(shape);
            shape = shape.substr(7, shape.length()-1);
            child_item->setText(3, QString::fromStdString(shape));
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

// slots
void RawTreeView::entry_double_clicked(QTreeWidgetItem* item, int column)
{
    if (item->childCount() == 0)
        std::cout << item->text(0).toStdString() << std::endl;
}

void RawTreeView::resize_to_content(QModelIndex qml)
{
    for (int c = 0; c<ui->treeWidget->columnCount();c++)
        ui->treeWidget->resizeColumnToContents(c);
}

void RawTreeView::item_info_requested(QTreeWidgetItem* item, int column)
{
    std::vector<std::string> nix_path;
    QTreeWidgetItem* current_item = item;
    // don't do anything
    if (current_item->text(0) == QString("Metadata") || current_item->text(0)==QString("Data"))
    {
        emit item_info_found(std::string("-"), std::string("-"), std::string("-"), std::string("-"));
        return;
    }

    // get full path to nix_file root
    while (current_item->text(0)!=QString("Metadata") && current_item->text(0)!=QString("Data"))
    {
        nix_path.push_back(current_item->text(0).toStdString());
        current_item = current_item->parent();
    }

    // get data info
    if (current_item->text(0)==QString("Data"))
    {
        nix::Block block = nix_file.getBlock(nix_path.back());
        nix_path.pop_back();

        if(nix_path.size() == 0) //block info requested
        {
            emit item_info_found(block.id(), block.type(), block.name(), block.definition());
            return;
        }

        else if(nix_path.size() == 1) //data array/tag/multitag requested
        {
            if (item->text(1) == QString("Data Array"))
            {
                nix::DataArray da = block.getDataArray(item->text(0).toStdString());
                emit item_info_found(da.id(), da.type(), da.name(), da.definition());
            }
            else if (item->text(1) == QString("Tag"))
            {
                nix::Tag tag = block.getTag(item->text(0).toStdString());
                emit item_info_found(tag.id(), tag.type(), tag.name(), tag.definition());
            }
            else if (item->text(1) == QString("MultiTag"))
            {
                nix::MultiTag mtag = block.getMultiTag(item->text(0).toStdString());
                emit item_info_found(mtag.id(), mtag.type(), mtag.name(), mtag.definition());
            }
        }
    }
    else if (current_item->text(0) == QString("Metadata"))
    {
        nix::Section section = nix_file.getSection(nix_path.back());
        nix_path.pop_back();

        if(nix_path.size() == 0)
        {
            emit item_info_found(section.id(), section.type(), section.name(), section.definition());
            return;
        }

        while(nix_path.size() > 1)
        {
            section = section.getSection(nix_path.back());
            nix_path.pop_back();
        }

        if(!section.hasProperty(nix_path.back())) //section info requested
        {
            section = section.getSection(nix_path.back());
            emit item_info_found(section.id(), section.type(), section.name(), section.definition());
        }
        else
        {
            nix::Property property = section.getProperty(nix_path.back());
            emit item_info_found(property.id(), property.name(), property.definition());
        }
    }

}

// getter
const QTreeWidget* RawTreeView::get_tree_widget()
{
    return ui->treeWidget;
}

// destructor
RawTreeView::~RawTreeView()
{
    delete ui;
}

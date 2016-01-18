#include "RawTreeView.hpp"
#include "ui_RawTreeView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"


RawTreeView::RawTreeView(NixDataModel* _nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RawTreeView)
{
    ui->setupUi(this);
    filter_mode = 0;
    nix_model = _nix_model;
    ui->treeView->setModel(nix_model);
    init_tree_widget();
}


void RawTreeView::init_tree_widget() {

}

//    if (!(filter_mode == 2))
//    {
//        QTreeWidgetItem* data_branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString("Data")));

//        for (nix::Block b : nix_file.blocks()) {
//            QTreeWidgetItem* tree_item = new QTreeWidgetItem(data_branch, QStringList(QString::fromStdString(b.name())));
//            tree_item->setText(1, QString::fromStdString(b.type()));

//            for (nix::DataArray da  : b.dataArrays()) {
//                if (!(filter_mode == 0) && !(filter_mode == 1) && !(filter_mode == 3))
//                    continue;

//                QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(da.name())));
//                child_item->setText(1, QString::fromStdString(da.type()));
//                child_item->setText(2, QString::fromStdString("Data Array"));
//                child_item->setText(3, QString::fromStdString(nix::data_type_to_string(da.dataType())));
//                std::stringstream s;
//                s << da.dataExtent();
//                std::string shape = s.str();
//                boost::algorithm::trim(shape);
//                shape = shape.substr(7, shape.length()-1);
//                child_item->setText(4, QString::fromStdString(shape));
//                add_linked_sources(child_item, QVariant::fromValue(da));
//            }

//            for (nix::Tag t : b.tags()) {
//                if (!(filter_mode == 4) && !(filter_mode == 0))
//                    continue;
//                QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(t.name())));
//                child_item->setText(1, QString::fromStdString(t.type()));
//                child_item->setText(2, QString::fromStdString("Tag"));
//                add_linked_sources(child_item, QVariant::fromValue(t));
//            }

//            for (nix::MultiTag m : b.multiTags()) {
//                if (!(filter_mode == 4) && !(filter_mode == 0))
//                    continue;
//                QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(m.name())));
//                child_item->setText(1, QString::fromStdString(m.type()));
//                child_item->setText(2, QString::fromStdString("MultiTag"));
//                add_linked_sources(child_item, QVariant::fromValue(b));
//            }

//            for (nix::Source s : b.sources())
//            {
//                if (!(filter_mode == 5) && !(filter_mode == 0))
//                    continue;
//                QTreeWidgetItem* child_item = new QTreeWidgetItem(tree_item, QStringList(QString::fromStdString(s.name())));
//                child_item->setText(1, QString::fromStdString(b.type()));
//                child_item->setText(2, QString::fromStdString("Source"));
//            }
//        }
//    }

//    if (filter_mode == 0 || filter_mode == 2)
//    {
//        QTreeWidgetItem* metadata_branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString("Metadata")));

//        for (nix::Section s : nix_file.sections()) {
//            QTreeWidgetItem* tree_item = new QTreeWidgetItem(metadata_branch, QStringList(QString::fromStdString(s.name())));
//            tree_item->setText(1, QString::fromStdString(s.type()));
//            add_children_to_item(tree_item, s);
//        }
//    }

//    expand();
//    for (int c = 0; c<ui->treeWidget->columnCount(); ++c)
//        ui->treeWidget->resizeColumnToContents(c);
//}

//void RawTreeView::add_linked_sources(QTreeWidgetItem* item, QVariant nix_item)
//{
//    if (nix_item.canConvert<nix::DataArray>())
//        add_linked_sources_helper(item, nix_item.value<nix::DataArray>());
//    else if (nix_item.canConvert<nix::Tag>())
//        add_linked_sources_helper(item, nix_item.value<nix::Tag>());
//    else if (nix_item.canConvert<nix::MultiTag>())
//        add_linked_sources_helper(item, nix_item.value<nix::MultiTag>());
//}

//template <typename T>
//void RawTreeView::add_linked_sources_helper(QTreeWidgetItem* item, T nix_item)
//{
//    for (nix::Source s : nix_item.sources())
//    {
//        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(s.name())));
//        child_item->setText(1, QString::fromStdString(s.type()));
//        child_item->setText(2, QString::fromStdString("Source/Link"));
//    }
//}

//void RawTreeView::add_children_to_item(QTreeWidgetItem* item, nix::Section section) {
//    for  (auto s : section.sections()) {
//        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(s.name())));
//        child_item->setText(1, QString::fromStdString(s.type()));
//        add_children_to_item(child_item, s);
//    }

//    for (nix::Property p : section.properties()) {
//        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(p.name())));
//        child_item->setText(2, QString::fromStdString("Metadata"));
//        child_item->setText(3, QString::fromStdString(nix::data_type_to_string(p.dataType())));
//    }
//}


// slots
void RawTreeView::resize_to_content(QModelIndex qml) {
    for (int c = 0; c<nix_model->columnCount();c++)
        ui->treeView->resizeColumnToContents(c);
}

//void RawTreeView::expand()
//{
//    if (filter_mode == 0 || filter_mode == 2)
//        ui->treeWidget->expandToDepth(0);
//    else
//        ui->treeWidget->expandToDepth(1);
//}

//void RawTreeView::currentItemChanged_worker(QTreeWidgetItem* old_item, QTreeWidgetItem* new_item)
//{
//    if (old_item)
//        item_info_requested(old_item, 0);
//}

//void RawTreeView::item_info_requested(QTreeWidgetItem* item, int column) {
//    std::vector<std::string> nix_path;
//    QTreeWidgetItem* current_item = item;
//    // don't do anything
//    if (current_item->text(0) == QString("Metadata") || current_item->text(0) == QString("Data")) {
//        // create empty block
//        emit empty_item();
//        return;
//    }

//    // get full path to nix_file root
//    while (current_item->text(0) != QString("Metadata") && current_item->text(0) != QString("Data")) {
//        nix_path.push_back(current_item->text(0).toStdString());
//        current_item = current_item->parent();
//    }

//    // get data info
//    if (current_item->text(0)==QString("Data")) {
//        nix::Block block = nix_file.getBlock(nix_path.back());
//        nix_path.pop_back();

//        if(nix_path.size() == 0) //block info requested
//        {
//            emit item_found(QVariant::fromValue(block));
//            return;
//        }
//        else  //data array/tag/multitag/source requested
//        {
//            if (item->text(2) == QString("Data Array")) {
//                nix::DataArray da = block.getDataArray(nix_path[0]);
//                emit item_found(QVariant::fromValue(da));
//            } else if (item->text(2) == QString("Tag")) {
//                nix::Tag tag = block.getTag(nix_path[0]);
//                emit item_found(QVariant::fromValue(tag));
//            } else if (item->text(2) == QString("MultiTag")) {
//                nix::MultiTag mtag = block.getMultiTag(nix_path[0]);
//                emit item_found(QVariant::fromValue(mtag));
//            } else if (item->text(2) == QString("Source")) {
//                nix::Source source = block.getSource(nix_path[0]);
//                emit item_found(QVariant::fromValue(source));
//            }else if (item->text(2) == QString("Source/Link")) {
//                nix::Source source = block.getSource(nix_path[0]);
//                emit item_found(QVariant::fromValue(source));
//            }
//        }
//    }
//    else if (current_item->text(0) == QString("Metadata")) {
//        nix::Section section = nix_file.getSection(nix_path.back());
//        nix_path.pop_back();

//        if(nix_path.size() == 0) {
//            emit item_found(QVariant::fromValue(section));
//            return;
//        }

//        while(nix_path.size() > 1) {
//            section = section.getSection(nix_path.back());
//            nix_path.pop_back();
//        }

//        if(!section.hasProperty(nix_path.back())) { //section info requested
//            section = section.getSection(nix_path.back());
//            emit item_found(QVariant::fromValue(section));
//        } else {
//            nix::Property property = section.getProperty(nix_path.back());
//            emit item_found(QVariant::fromValue(property));
//        }
//    }
//}

//void RawTreeView::filter_changed(QString _mode)
//{
//    std::string mode = _mode.toStdString();
//    boost::algorithm::to_lower(mode);
//    if (mode == "-")
//        filter_mode = 0;
//    else if (mode == "data")
//        filter_mode = 1;
//    else if (mode == "metadata")
//        filter_mode = 2;
//    else if (mode == "data array")
//        filter_mode = 3;
//    else if (mode == "tags")
//        filter_mode = 4;
//    else if (mode == "sources")
//        filter_mode = 5;

//    refresh();
//}

//void RawTreeView::refresh()
//{
//    ui->treeWidget->clear();
//    init_tree_widget();
//    emit empty_item();
//}

// getter
const QTreeView* RawTreeView::get_tree_view()
{
    return ui->treeView;
}
//const QComboBox* RawTreeView::get_filter_combo_box()
//{
//    return ui->comboBox;
//}


// destructor
RawTreeView::~RawTreeView()
{
    delete ui;
}

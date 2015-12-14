#include "NixDataModel.hpp"
#include <iostream>
#include <nix.hpp>
#include <boost/algorithm/string.hpp>

typedef QList<QStandardItem*> Row;
typedef QList<QString> RowStrings;

NixDataModel::NixDataModel(nix::File _nix_file) :
    QStandardItemModel()
{
    nix_file = _nix_file;

    RowStrings headers;
    headers << "Name" << "Nix Type" << "Storage Type" << "Data Type" << "Shape" << "ID" << "CreatedAt" << "UpdatedAt";
    setHorizontalHeaderLabels(headers);

    nix_file_to_model();
}

void NixDataModel::nix_file_to_model() {
    QStandardItem* root_node = this->invisibleRootItem();

    RowStrings data_list;
    data_list << "Data";
    Row data_branch = create_entry_row(data_list);
    root_node->appendRow(data_branch);

    for (nix::Block b : nix_file.blocks()) {
        RowStrings block_list;
        block_list << s_to_q(b.name())
                   << s_to_q(b.type());
        Row b_m = create_entry_row(block_list);
        data_branch.first()->appendRow(b_m);

        for (nix::DataArray da  : b.dataArrays()) {
            RowStrings da_list;
            std::stringstream s;
            s << da.dataExtent();
            std::string shape = s.str();
            boost::algorithm::trim(shape);
            shape = shape.substr(7, shape.length()-1);
            da_list << s_to_q(da.name())
                    << s_to_q(da.type())
                    << "Data Array"
                    << s_to_q(nix::data_type_to_string(da.dataType()))
                    << s_to_q(shape)
                    << s_to_q(da.id())
                    << s_to_q(get_created_at(da))
                    << s_to_q(get_updated_at(da));
            Row da_m = create_entry_row(da_list);
            b_m.first()->appendRow(da_m);
//            add_linked_sources(child_item, QVariant::fromValue(da));
        }

        for (nix::Tag t : b.tags()) {
            RowStrings t_list;
            t_list << s_to_q(t.name())
                   << s_to_q(t.type())
                   << "Tag"
                   << ""
                   << ""
                   << s_to_q(t.id())
                   << s_to_q(get_created_at(t))
                   << s_to_q(get_updated_at(t));
            Row t_m = create_entry_row(t_list);
            b_m.first()->appendRow(t_m);
        }

        for (nix::MultiTag m : b.multiTags()) {
            RowStrings m_list;
            m_list << s_to_q(m.name())
                   << s_to_q(m.type())
                   << "Multitag"
                   << ""
                   << ""
                   << s_to_q(m.id())
                   << s_to_q(get_created_at(m))
                   << s_to_q(get_updated_at(m));
            Row m_m = create_entry_row(m_list);
            b_m.first()->appendRow(m_m);
//            add_linked_sources(child_item, QVariant::fromValue(b));
        }

        for (nix::Source s : b.sources())
        {
            RowStrings s_list;
            s_list << s_to_q(s.name())
                   << s_to_q(s.type())
                   << "Source"
                   << ""
                   << ""
                   << s_to_q(s.id())
                   << s_to_q(get_created_at(s))
                   << s_to_q(get_updated_at(s));
            Row s_m = create_entry_row(s_list);
            b_m.first()->appendRow(s_m);
        }
    }

    RowStrings metadata_list;
    metadata_list << "MetaData";
    Row metadata_branch = create_entry_row(metadata_list);
    root_node->appendRow(metadata_branch);

    for (nix::Section s : nix_file.sections()) {
        RowStrings s_list;
        s_list << s_to_q(s.name())
               << s_to_q(s.type())
               << "Source"
               << ""
               << ""
               << s_to_q(s.id())
               << s_to_q(get_created_at(s))
               << s_to_q(get_updated_at(s));
        Row s_m = create_entry_row(s_list);
        metadata_branch.first()->appendRow(s_m);

//        add_children_to_item(tree_item, s);
    }
}

//TODO
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

Row NixDataModel::create_entry_row(RowStrings row_entries)
{
    Row row_items;
    for (QString entry : row_entries)
    {
        row_items << new QStandardItem(entry);
    }
    return row_items;
}

QString NixDataModel::s_to_q(std::string s)
{
    return QString::fromStdString(s);
}

template<typename T>
std::string NixDataModel::get_created_at(T arg)
{
    time_t rawtime_cr = arg.createdAt();
    struct tm *info_cr;
    info_cr = localtime( &rawtime_cr );
    return boost::algorithm::trim_right_copy(std::string(asctime(info_cr)));
}

template<typename T>
std::string NixDataModel::get_updated_at(T arg)
{
    time_t rawtime_cr = arg.updatedAt();
    struct tm *info_cr;
    info_cr = localtime( &rawtime_cr );
    return boost::algorithm::trim_right_copy(std::string(asctime(info_cr)));
}

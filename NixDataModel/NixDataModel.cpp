#include "NixDataModel.hpp"
#include <iostream>
#include <nix.hpp>
#include <boost/algorithm/string.hpp>

NixDataModel::NixDataModel(nix::File _nix_file) :
    QStandardItemModel()
{
    nix_file = _nix_file;

    RowStrings headers;
    headers << "Name"               // 0
            << "Nix Type"           // 1
            << "Storage Type"       // 2
            << "Data Type"          // 3
            << "Shape"              // 4
            << "ID"                 // 5
            << "CreatedAt"          // 6
            << "UpdatedAt"          // 7
            << "Value"              // 8
            << "root_child_link";   // 9
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
        RowStrings block_list = create_rowstrings(b, "Block", "root", b.type(), (std::string)"");
        Row b_m = create_entry_row(block_list);
        data_branch.first()->appendRow(b_m);

        for (nix::Group g : b.groups())
        {
            RowStrings group_list = create_rowstrings(g, "Group", "child", g.type(), (std::string)"");
            Row g_m = create_entry_row(group_list);
            add_content(g_m.first(), g);
        }

        add_content(b_m.first(), b);
        add_linked_metadata(b_m.first(), b);
    }

    RowStrings metadata_list;
    metadata_list << "MetaData";
    Row metadata_branch = create_entry_row(metadata_list);
    root_node->appendRow(metadata_branch);

    for (nix::Section s : nix_file.sections()) {
        RowStrings s_list = create_rowstrings(s, "Section", "root");
        Row s_m = create_entry_row(s_list);
        metadata_branch.first()->appendRow(s_m);

        add_subsec_prop(s_m.first(), s);
    }
}

template<typename T>
void NixDataModel::add_content(QStandardItem* item, T nix_entity)
{
    for (nix::DataArray da  : nix_entity.dataArrays()) {
        std::stringstream s;
        s << da.dataExtent();
        std::string shape = s.str();
        boost::algorithm::trim(shape);
        shape = shape.substr(7, shape.length()-1);
        RowStrings da_list = create_rowstrings(da, "Data Array", "child", nix::data_type_to_string(da.dataType()), shape);
        Row da_m = create_entry_row(da_list);
        item->appendRow(da_m);
        add_linked_sources(da_m.first(), da);
        add_linked_metadata(da_m.first(), da);
    }

    for (nix::Tag t : nix_entity.tags()) {
        RowStrings t_list = create_rowstrings(t, "Tag", "child");
        Row t_m = create_entry_row(t_list);
        item->appendRow(t_m);
        add_linked_metadata(t_m.first(), t);
    }

    for (nix::MultiTag m : nix_entity.multiTags()) {
        RowStrings m_list = create_rowstrings(m, "Multitag", "child");
        Row m_m = create_entry_row(m_list);
        item->appendRow(m_m);
        add_linked_sources(m_m.first(), m);
        add_linked_metadata(m_m.first(), m);
    }

    for (nix::Source s : nix_entity.sources())
    {
        RowStrings s_list = create_rowstrings(s, "Source", "child");
        Row s_m = create_entry_row(s_list);
        item->appendRow(s_m);
        add_linked_metadata(s_m.first(), s);
    }
}

void NixDataModel::add_subsec_prop(QStandardItem* item, nix::Section section) {
    for  (auto s : section.sections()) {
        RowStrings s_list = create_rowstrings(s, "Section", "child");
        Row s_m = create_entry_row(s_list);
        item->appendRow(s_m);

        add_subsec_prop(s_m.first(), s);
    }

    for (nix::Property p : section.properties()) {
        RowStrings p_list;
        p_list << s_to_q(p.name())
               << "Property"
               << s_to_q(nix::data_type_to_string(p.dataType()))
               << ""
               << ""
               << s_to_q(p.id())
               << s_to_q(get_created_at(p))
               << s_to_q(get_updated_at(p))
               << ""
               << "child";
        Row p_m = create_entry_row(p_list);
        item->appendRow(p_m);
    }
}

template<typename T>
void NixDataModel::add_linked_sources(QStandardItem* item, T nix_entity)
{
    for (nix::Source s : nix_entity.sources())
    {
        RowStrings s_list = create_rowstrings(s, "Link/Source", "link");
        Row s_m = create_entry_row(s_list);
        item->appendRow(s_m);
    }
}

template<typename T>
void NixDataModel::add_linked_metadata(QStandardItem* item, T nix_entity)
{
    nix::Section s = nix_entity.metadata();
    if (s)
    {
        RowStrings s_list = create_rowstrings(s, "Link/Metadata", "link");
        Row s_m = create_entry_row(s_list);
        item->appendRow(s_m);
    }
}

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
RowStrings NixDataModel::create_rowstrings(T arg, std::string storagetype, std::string root_child_link, std::string nixtype, std::string shape)
{
    RowStrings s_list;
    s_list << s_to_q(arg.name())
           << s_to_q(arg.type())
           << s_to_q(storagetype)
           << s_to_q(nixtype)
           << s_to_q(shape)
           << s_to_q(arg.id())
           << s_to_q(get_created_at(arg))
           << s_to_q(get_updated_at(arg))
           << ""
           << s_to_q(root_child_link);
    return s_list;
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

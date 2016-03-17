#include "NixDataModel.hpp"
#include <iostream>
#include <nix.hpp>
#include <boost/algorithm/string.hpp>
#include "common/Common.hpp"
#include <math.h>

NixDataModel::NixDataModel() : QStandardItemModel(){
    RowStrings headers;
    headers << MODEL_HEADER_NAME               //  0
            << MODEL_HEADER_NIXTYPE            //  1
            << MODEL_HEADER_STORAGETYPE        //  2
            << MODEL_HEADER_DATATYPE           //  3
            << MODEL_HEADER_SHAPE              //  4
            << MODEL_HEADER_ID                 //  5
            << MODEL_HEADER_CREATEDAT          //  6
            << MODEL_HEADER_UPDATEDAT          //  7
            << MODEL_HEADER_VALUE              //  8
            << MODEL_HEADER_UNERTAINTY         //  9
            << MODEL_HEADER_ROOTCHILDLINK;     // 10
    setHorizontalHeaderLabels(headers);
    num_columns = headers.size();
}

NixDataModel::NixDataModel(const nix::File &nix_file) :
    NixDataModel()
{
    nix_file_to_model(nix_file);
}

void NixDataModel::nix_file_to_model(const nix::File &nix_file) {
    scan_progress = 0.0;
    emit file_scan_progress();
    QStandardItem* root_node = this->invisibleRootItem();

    RowStrings data_list;
    data_list << "Data";
    fill_rowstrings(data_list);
    Row data_branch = create_entry_row(data_list);
    root_node->appendRow(data_branch);
    double max_progress = 100 / nix_file.blockCount();
    for (nix::Block b : nix_file.blocks()) {
        nix::ndsize_t top_level_count = b.dataArrayCount() + b.multiTagCount() + b.groupCount() + b.tagCount() + b.sourceCount();
        double prog_incr = max_progress/top_level_count;
        RowStrings block_list = create_rowstrings(b, NIX_STRING_BLOCK, "root", b.type(), (std::string)"");
        Row b_m = create_entry_row(block_list, b);
        data_branch.first()->appendRow(b_m);

        for (nix::Group g : b.groups())
        {
            RowStrings group_list = create_rowstrings(g, NIX_STRING_GROUP, "child", g.type(), (std::string)"");
            Row g_m = create_entry_row(group_list, g);
            add_content(g_m.first(), g, 0);
            scan_progress += prog_incr;
            emit file_scan_progress();
        }

        add_content(b_m.first(), b, prog_incr);
        add_linked_metadata(b_m.first(), b);
    }

    RowStrings metadata_list;
    metadata_list << NIX_STRING_METADATA;
    fill_rowstrings(metadata_list);
    Row metadata_branch = create_entry_row(metadata_list);
    root_node->appendRow(metadata_branch);

    for (nix::Section s : nix_file.sections()) {
        RowStrings s_list = create_rowstrings(s, NIX_STRING_SECTION, "root");
        Row s_m = create_entry_row(s_list, s);
        metadata_branch.first()->appendRow(s_m);

        add_subsec_prop(s_m.first(), s);
    }
    scan_progress = 100.0;
    emit file_scan_progress();
}

template<typename T>
void NixDataModel::add_content(QStandardItem* item, T nix_entity, double prog_incr)
{
    for (nix::DataArray da : nix_entity.dataArrays()) {
        std::stringstream s;
        s << da.dataExtent();
        std::string shape = s.str();
        boost::algorithm::trim(shape);
        shape = shape.substr(7, shape.length()-1);
        RowStrings da_list = create_rowstrings(da, NIX_STRING_DATAARRAY, "child", nix::data_type_to_string(da.dataType()), shape);
        Row da_m = create_entry_row(da_list, da);
        item->appendRow(da_m);
        add_linked_sources(da_m.first(), da);
        add_linked_metadata(da_m.first(), da);
        scan_progress += prog_incr;
        emit file_scan_progress();
    }

    for (nix::Tag t : nix_entity.tags()) {
        RowStrings t_list = create_rowstrings(t, NIX_STRING_TAG, "child");
        Row t_m = create_entry_row(t_list, t);
        item->appendRow(t_m);
        for(nix::DataArray da : t.references())
        {
            RowStrings da_list = create_rowstrings(da, REFERENCE_DATAARRAY, "link");
            Row test = create_entry_row(da_list, da);
            t_m.first()->appendRow(test);
        }
        for(nix::Feature f : t.features())
        {
            RowStrings da_list = create_rowstrings(f.data(), FEATURE_DATAARRAY, "link");
            Row test = create_entry_row(da_list, f);
            t_m.first()->appendRow(test);
        }
        add_linked_metadata(t_m.first(), t);
        scan_progress += prog_incr;
        emit file_scan_progress();
    }

    for (nix::MultiTag m : nix_entity.multiTags()) {
        RowStrings m_list = create_rowstrings(m, NIX_STRING_MULTITAG, "child");
        Row m_m = create_entry_row(m_list, m);
        item->appendRow(m_m);
        add_linked_sources(m_m.first(), m);
        add_linked_metadata(m_m.first(), m);
        scan_progress += prog_incr;
        emit file_scan_progress();
    }

    for (nix::Source s : nix_entity.sources())
    {
        RowStrings s_list = create_rowstrings(s, NIX_STRING_SOURCE, "child");
        Row s_m = create_entry_row(s_list, s);
        item->appendRow(s_m);
        add_linked_metadata(s_m.first(), s);
        scan_progress += prog_incr;
        emit file_scan_progress();
    }
}

void NixDataModel::add_subsec_prop(QStandardItem* item, nix::Section section) {
    for  (auto s : section.sections()) {
        RowStrings s_list = create_rowstrings(s, NIX_STRING_SECTION, "child");
        Row s_m = create_entry_row(s_list, s);
        item->appendRow(s_m);

        add_subsec_prop(s_m.first(), s);
    }

    for (nix::Property p : section.properties()) {
        std::string v = get_property_value(p);
        std::string u = get_property_uncertainty(p);
        RowStrings p_list;
        p_list << s_to_q(p.name())                                  //  0
               << NIX_STRING_PROPERTY                               //  1
               << s_to_q(nix::data_type_to_string(p.dataType()))    //  2
               << ""                                                //  3
               << ""                                                //  4
               << s_to_q(p.id())                                    //  5
               << s_to_q(get_created_at(p))                         //  6
               << s_to_q(get_updated_at(p))                         //  7
               << s_to_q(v)                                         //  8
               << s_to_q(u)                                         //  9
               << "child";                                          // 10
        Row p_m = create_entry_row(p_list, p);
        item->appendRow(p_m);
    }
}

std::string NixDataModel::get_property_value(nix::Property p)
{
    std::vector<nix::Value> values = p.values();
    std::string v_type = nix::data_type_to_string(p.dataType());
    std::ostringstream oss;
    for (int i = 0; i < (int)values.size(); ++i)
    {
        if (v_type == NIX_STRING_TYPE_STRING) {
            std::string value;
            values[i].get(value);
            oss << value;
        } else if (v_type == NIX_STRING_TYPE_BOOL){
            bool value;
            values[i].get(value);
            oss << value;
        } else if (v_type == NIX_STRING_TYPE_INT32){
            int32_t value;
            values[i].get(value);
            oss << value;
        } else if (v_type == NIX_STRING_TYPE_INT64){
            int64_t value;
            values[i].get(value);
            oss << value;
        } else if (v_type == NIX_STRING_TYPE_UINT64){
            uint64_t value;
            values[i].get(value);
            oss << value;
        } else if (v_type == NIX_STRING_TYPE_DOUBLE){
            double value;
            values[i].get(value);
            oss << value;
        } else {
            oss << "NOT READABLE";
        }

        if (i < (int)values.size()-1)
            oss << ", ";
    }
    return oss.str();
}

std::string NixDataModel::get_property_uncertainty(nix::Property p)
{
    std::vector<nix::Value> values = p.values();
    std::string v_type = nix::data_type_to_string(p.dataType());
    std::ostringstream oss;
    for (int i = 0; i < (int)values.size(); ++i)
    {
        if (v_type == NIX_STRING_TYPE_STRING) {
            std::string value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else if (v_type == NIX_STRING_TYPE_BOOL){
            bool value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else if (v_type == NIX_STRING_TYPE_INT32){
            int32_t value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else if (v_type == NIX_STRING_TYPE_INT64){
            int64_t value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else if (v_type == NIX_STRING_TYPE_UINT64){
            uint64_t value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else if (v_type == NIX_STRING_TYPE_DOUBLE){
            double value;
            values[i].get(value);
            oss << values[i].uncertainty;
        } else {
            oss << "NOT READABLE";
        }

        if (i < (int)values.size()-1)
            oss << ", ";
    }
    return oss.str();
}

template<typename T>
void NixDataModel::add_linked_sources(QStandardItem* item, T nix_entity)
{
    for (nix::Source s : nix_entity.sources())
    {
        RowStrings s_list = create_rowstrings(s, "Link/Source", "link");
        Row s_m = create_entry_row(s_list, nix_entity);
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
        Row s_m = create_entry_row(s_list, nix_entity);
        item->appendRow(s_m);
    }
}

template<typename T>
Row NixDataModel::create_entry_row(RowStrings row_entries, T nix_entity)
{
    Row row_items;

    for (QString entry : row_entries)
    {
        NixModelItem *item = new NixModelItem(entry, QVariant::fromValue(nix_entity));
        row_items << static_cast<QStandardItem*>(item);
    }
    return row_items;
}

Row NixDataModel::create_entry_row(RowStrings row_entries)
{
    Row row_items;
    for (QString entry : row_entries)
    {
        NixModelItem *item = new NixModelItem(entry);
        row_items << static_cast<QStandardItem*>(item);
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
    s_list << s_to_q(arg.name())            //  0
           << s_to_q(arg.type())            //  1
           << s_to_q(storagetype)           //  2
           << s_to_q(nixtype)               //  3
           << s_to_q(shape)                 //  4
           << s_to_q(arg.id())              //  5
           << s_to_q(get_created_at(arg))   //  6
           << s_to_q(get_updated_at(arg))   //  7
           << ""                            //  8
           << ""                            //  9
           << s_to_q(root_child_link);      // 10
    return s_list;
}

void NixDataModel::fill_rowstrings(RowStrings &rowstrings)
{
    for(int i = rowstrings.size(); i < num_columns; ++i)
        rowstrings << "";
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

NixModelItem* NixDataModel::get_item_from_qml(QModelIndex qml)
{
    return static_cast<NixModelItem*>(itemFromIndex(qml));
}

int NixDataModel::progress()
{
    return rint(this->scan_progress);
}

NixDataModel::~NixDataModel()
{
}

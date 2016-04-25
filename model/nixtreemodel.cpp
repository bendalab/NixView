#include "nixtreemodel.h"
#include <common/Common.hpp>

NixTreeModel::NixTreeModel(QObject *parent)
    : QAbstractItemModel(parent) {
    root_item = new NixTreeModelItem("");
}


NixTreeModel::~NixTreeModel() {
    if (root_item != nullptr) {
        delete data_node;
        delete metadata_node;
        delete root_item;
    }
}


void NixTreeModel::set_entity(const nix::File &file) {
    this->file = file;
    if (root_item != nullptr) {
        delete root_item;
    }
    root_item = new NixTreeModelItem("Root");
    data_node = new NixTreeModelItem("Data", root_item);
    metadata_node = new NixTreeModelItem("Metadata", root_item);
    root_item->appendChild(data_node);
    root_item->appendChild(metadata_node);
    fetchL1Blocks(file);
    fetchL1Sections(file);
}


void NixTreeModel::fetchL1Blocks(const nix::File &file) {
    for (nix::Block b: file.blocks()) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(b), data_node);
        data_node->appendChild(itm);
    }
}


void NixTreeModel::fetchL1Sections(const nix::File &file) {
    for (nix::Section s: file.sections()) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(s), metadata_node);
        metadata_node->appendChild(itm);
    }
}




QVariant NixTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Orientation::Horizontal) {
        return QVariant(this->root_item->getHeader(section));
    }
    return QVariant("");
}


QModelIndex NixTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    NixTreeModelItem *parentItem;

    if (!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<NixTreeModelItem*>(parent.internalPointer());

    NixTreeModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


QModelIndex NixTreeModel::parent(const QModelIndex &index) const{
    if (!index.isValid())
          return QModelIndex();

    NixTreeModelItem *childItem = static_cast<NixTreeModelItem*>(index.internalPointer());
    NixTreeModelItem *parentItem = childItem->parentItem();
    if (parentItem == root_item || parentItem == nullptr)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


int NixTreeModel::rowCount(const QModelIndex &parent) const {
    NixTreeModelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = root_item;
    else
        parentItem = static_cast<NixTreeModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}


int NixTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<NixTreeModelItem*>(parent.internalPointer())->columnCount();
    else
        return root_item->columnCount();
}


QVariant NixTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(index.internalPointer());

    return item->data(index.column());
}


bool NixTreeModel::hasChildren(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return true;
    }

    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(parent.internalPointer());
    NixType nix_type = item->nixType();
    if (nix_type == NixType::NIX_UNKNOWN) {
        if (item->data(0) == "Data") {
            return file.blockCount() > 0;
        } else if (item->data(0) == "Metadata") {
            return file.sectionCount() > 0;
        }
    }
    return checkForKids(item) > 0;
}


bool NixTreeModel::canFetchMore(const QModelIndex &parent) const {
    if (!parent.isValid())
        return false;
    NixTreeModelItem *itm = static_cast<NixTreeModelItem*>(parent.internalPointer());
    return checkForKids(itm) != rowCount(parent);
}


void NixTreeModel::fetchMore(const QModelIndex &parent) {
    if (!parent.isValid()) {
        return;
    }
    NixTreeModelItem *itm = static_cast<NixTreeModelItem*>(parent.internalPointer());
    std::cerr << "fetch more!" << std::endl;
    switch (itm->nixType()) {
        case NixType::NIX_BLOCK: {
            fetch_block(itm->itemData().value<nix::Block>(), itm);
            break;
        }
        case NixType::NIX_DATA_ARRAY: {
            fetch_data_array(itm->itemData().value<nix::DataArray>(), itm);
            break;
        }
        case NixType::NIX_TAG: {
            fetch_tag(itm->itemData().value<nix::Tag>(), itm);
            break;
        }
        case NixType::NIX_MTAG: {
            fetch_multi_tag(itm->itemData().value<nix::MultiTag>(), itm);
            break;
        }
        case NixType::NIX_GROUP: {
            fetch_group(itm->itemData().value<nix::Group>(), itm);
            break;
        }
        case NixType::NIX_SECTION: {
            fetch_section(itm->itemData().value<nix::Section>(), itm);
        }
        default:
            break;
    }
}


int NixTreeModel::checkForKids(NixTreeModelItem *item) const {
    NixType nix_type = item->nixType();
    if (nix_type == NixType::NIX_DIMENSION || nix_type == NixType::NIX_PROPERTY || nix_type == NixType::NIX_FEAT)
        return 0;
    switch (nix_type) {
        case NixType::NIX_BLOCK: {
            nix::Block b = item->itemData().value<nix::Block>();
            return b.dataArrayCount() + b.groupCount()  + b.tagCount() + b.multiTagCount() + b.sourceCount();
        }
        case NixType::NIX_DATA_ARRAY: {
            nix::DataArray a = item->itemData().value<nix::DataArray>();
            return a.dimensionCount();
        }
        case NixType::NIX_GROUP: {
            nix::Group g = item->itemData().value<nix::Group>();
            return g.dataArrayCount() + g.tagCount() + g.multiTagCount();
        }
        case NixType::NIX_TAG: {
            nix::Tag t = item->itemData().value<nix::Tag>();
            return t.referenceCount() + t.featureCount();
        }
        case NixType::NIX_MTAG: {
            nix::MultiTag mt = item->itemData().value<nix::MultiTag>();
            return mt.referenceCount() + mt.featureCount();
        }
        case NixType::NIX_SECTION: {
            nix::Section s = item->itemData().value<nix::Section>();
            return s.propertyCount();
        }
        case NixType::NIX_SOURCE: {
            nix::Source src = item->itemData().value<nix::Source>();
            return src.sourceCount();
        }
        default:
            return 0;
    }
}


void NixTreeModel::fetch_block(const nix::Block &b, NixTreeModelItem *parent) {
    append_data_arrays(b.dataArrays(), parent);
    append_groups(b.groups(), parent);
    append_tags(b.tags(), parent);
    append_multi_tags(b.multiTags(), parent);
    append_sources(b.sources(), parent);
}


void NixTreeModel::fetch_data_array(const nix::DataArray &da, NixTreeModelItem *parent) {
    for (nix::Dimension d : da.dimensions()) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(d), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::fetch_tag(const nix::Tag &t, NixTreeModelItem *parent) {
    append_data_arrays(t.references(), parent);
    append_features(t.features(), parent);
}


void NixTreeModel::fetch_multi_tag(const nix::MultiTag &t, NixTreeModelItem *parent) {
    append_data_arrays(t.references(), parent);
    append_features(t.features(), parent);
}


void NixTreeModel::append_features(const std::vector<nix::Feature> &feats, NixTreeModelItem *parent) {
    for (nix::Feature f : feats) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(f), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::fetch_group(const nix::Group &g, NixTreeModelItem *parent) {
    append_data_arrays(g.dataArrays(), parent);
    append_tags(g.tags(), parent);
    append_multi_tags(g.multiTags(), parent);
}


void NixTreeModel::fetch_section(const nix::Section &s, NixTreeModelItem *parent) {
    append_properties(s.properties(), parent);
    append_sections(s.sections(), parent);
}


void NixTreeModel::fetch_source(const nix::Source &s, NixTreeModelItem *parent) {
    append_sources(s.sources(), parent);
}


void NixTreeModel::append_groups(const std::vector<nix::Group> &groups, NixTreeModelItem *parent) {
    for (nix::Group g : groups) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(g), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_data_arrays(const std::vector<nix::DataArray> &arrays, NixTreeModelItem *parent) {
    for (nix::DataArray d : arrays) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(d), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_tags(const std::vector<nix::Tag> &tags, NixTreeModelItem *parent) {
    for (nix::Tag t : tags) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(t), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_multi_tags(const std::vector<nix::MultiTag> &tags, NixTreeModelItem *parent) {
    for (nix::MultiTag t : tags) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(t), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_sections(const std::vector<nix::Section> &sections, NixTreeModelItem *parent) {
    for (nix::Section s : sections) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(s), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_properties(const std::vector<nix::Property> &props, NixTreeModelItem *parent) {
    for (nix::Property p : props) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(p), parent);
        parent->appendChild(itm);
    }
}


void NixTreeModel::append_sources(const std::vector<nix::Source> &sources, NixTreeModelItem *parent) {
    for (nix::Source s : sources) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(s), parent);
        parent->appendChild(itm);
    }
}

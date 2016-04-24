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
    //setup_data_model(file);
    //setup_metadata_model(file);
}


void NixTreeModel::setup_data_model(const nix::File &file) {
    for (nix::Block b: file.blocks()) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(b), data_node);
        data_node->appendChild(itm);
        append_groups(b, itm);
        append_data_arrays(b.dataArrays(), itm);
        append_tags(b.tags(), itm);
        append_multi_tags(b.multiTags(), itm);
    }
}


void NixTreeModel::append_groups(const nix::Block &b, NixTreeModelItem *parent) {
    for (nix::Group g : b.groups()) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(g), parent);
        parent->appendChild(itm);
        append_data_arrays(g.dataArrays(), itm);
        append_tags(g.tags(), itm);
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


void NixTreeModel::setup_metadata_model(const nix::File &file) {

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

    if (nix_type == NixType::NIX_DIMENSION || nix_type == NixType::NIX_PROPERTY || nix_type == NixType::NIX_FEAT)
        return false;
    switch (nix_type) {
        case NixType::NIX_BLOCK: {
            nix::Block b = item->itemData().value<nix::Block>();
            return ((b.dataArrayCount() + b.groupCount()  + b.tagCount() + b.multiTagCount() + b.sourceCount()) > 0);
        }
        case NixType::NIX_DATA_ARRAY: {
            return true;
        }
        case NixType::NIX_GROUP: {
            nix::Group g = item->itemData().value<nix::Group>();
            return (g.dataArrayCount() + g.tagCount() + g.multiTagCount()) > 0;
        }
        case NixType::NIX_TAG: {
            nix::Tag t = item->itemData().value<nix::Tag>();
            return (t.referenceCount() + t.featureCount()) > 0;
        }
        case NixType::NIX_MTAG: {
            nix::MultiTag mt = item->itemData().value<nix::MultiTag>();
            return (mt.referenceCount() + mt.featureCount()) > 0;
        }
        case NixType::NIX_SECTION: {
            nix::Section s = item->itemData().value<nix::Section>();
            return s.propertyCount() > 0;
        }
        case NixType::NIX_SOURCE: {
            nix::Source src = item->itemData().value<nix::Source>();
            return src.sourceCount() > 0;
        }
        default:
            return false;
    }
    return false;
}


bool NixTreeModel::canFetchMore(const QModelIndex &parent) const {
    if (!parent.isValid())
        return false;
    std::cerr << "can fetch more?" << std::endl;
    NixTreeModelItem *itm = static_cast<NixTreeModelItem*>(parent.internalPointer());
    QString store_type = itm->data(2).toString();

    return (store_type == NIX_STRING_BLOCK || store_type == NIX_STRING_DATAARRAY || store_type == NIX_STRING_GROUP);
    //std::cerr << "can fetch more? " << itm->data(0).toString().toStdString() << std::endl;
    //return true;
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
    }
    }
}


void NixTreeModel::fetch_block(const nix::Block &b, NixTreeModelItem *parent) {

}

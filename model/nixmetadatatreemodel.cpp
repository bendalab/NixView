#include "nixmetadatatreemodel.h"
#include "common/Common.hpp"

NixMetadataTreeModel::NixMetadataTreeModel(QObject *parent)
    : QAbstractItemModel(parent) {
    rootItem = new NixTreeModelItem("");
}


NixMetadataTreeModel::~NixMetadataTreeModel() {
    delete rootItem;
}

void NixMetadataTreeModel::setEntity(const nix::Section &section) {
    if (rootItem != nullptr) {
        delete rootItem;
    }
    rootItem = new NixTreeModelItem("Root");
    rootItem->appendChild(new NixTreeModelItem(QVariant::fromValue(section), rootItem));
}


QVariant NixMetadataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Orientation::Horizontal) {
        return QVariant(this->rootItem->getHeader(section));
    }
    return QVariant();
 }


QModelIndex NixMetadataTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    NixTreeModelItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NixTreeModelItem*>(parent.internalPointer());

    NixTreeModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


QModelIndex NixMetadataTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
          return QModelIndex();

    NixTreeModelItem *childItem = static_cast<NixTreeModelItem*>(index.internalPointer());
    NixTreeModelItem *parentItem = childItem->parentItem();
    if (parentItem == rootItem || parentItem == nullptr)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


int NixMetadataTreeModel::rowCount(const QModelIndex &parent) const {
    NixTreeModelItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<NixTreeModelItem*>(parent.internalPointer());

    return parentItem->childCount();
}


int NixMetadataTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<NixTreeModelItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}


QVariant NixMetadataTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ToolTipRole)) {
        return QVariant();
    }
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    }
    return item->toolTip();
}


bool NixMetadataTreeModel::hasChildren(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return true;
    }

    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(parent.internalPointer());
    return checkForKids(item) > 0;
}


bool NixMetadataTreeModel::canFetchMore(const QModelIndex &parent) const {
    if (!parent.isValid())
        return false;
    NixTreeModelItem *itm = static_cast<NixTreeModelItem*>(parent.internalPointer());
    return checkForKids(itm) != rowCount(parent);
}


void NixMetadataTreeModel::fetchMore(const QModelIndex &parent) {
    if (!parent.isValid()) {
        return;
    }
    NixTreeModelItem *itm = static_cast<NixTreeModelItem*>(parent.internalPointer());
    if (itm->nixType() == NixType::NIX_SECTION) {
            fetchSection(itm->itemData().value<nix::Section>(), itm);
    }
}


int NixMetadataTreeModel::checkForKids(NixTreeModelItem *item) const {
    NixType nix_type = item->nixType();
    if (nix_type == NixType::NIX_DIMENSION || nix_type == NixType::NIX_PROPERTY || nix_type == NixType::NIX_FEAT)
        return 0;
    switch (nix_type) {
        case NixType::NIX_SECTION: {
            nix::Section s = item->itemData().value<nix::Section>();
            return s.propertyCount() + s.sectionCount();
        }
        default:
            return 0;
    }
}

void NixMetadataTreeModel::fetchSection(const nix::Section &s, NixTreeModelItem *parent) {
    appendProperties(s.properties(), parent);
    appendSections(s.sections(), parent);
}


void NixMetadataTreeModel::appendSections(const std::vector<nix::Section> &sections, NixTreeModelItem *parent) {
    for (nix::Section s : sections) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(s), parent);
        parent->appendChild(itm);
    }
}


void NixMetadataTreeModel::appendProperties(const std::vector<nix::Property> &props, NixTreeModelItem *parent) {
    for (nix::Property p : props) {
        NixTreeModelItem *itm = new NixTreeModelItem(QVariant::fromValue(p), parent);
        parent->appendChild(itm);
    }
}

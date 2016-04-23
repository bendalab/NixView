#include "nixtreemodel.h"
#include <common/Common.hpp>

NixTreeModel::NixTreeModel(QObject *parent)
    : QAbstractItemModel(parent) {
    root_item = new NixTreeModelItem("");
}


NixTreeModel::~NixTreeModel() {
    if (root_item != nullptr) {
        delete root_item;
    }

}


void NixTreeModel::set_entity(const nix::File &file) {
    if (root_item != nullptr) {
        delete root_item;
    }
    root_item = new NixTreeModelItem("Test");
    setup_model(file);
}


void NixTreeModel::setup_model(const nix::File &file) {
    for (nix::Block b: file.blocks()) {
        root_item->appendChild(new NixTreeModelItem(QVariant(QString::fromStdString(b.name()))));
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

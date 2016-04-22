#include "nixtreemodelitem.h"


NixTreeModelItem::NixTreeModelItem(const QVariant &data, NixTreeModelItem *parent) {
    this->parent_item = parent;
    this->item_data = data;
}


NixTreeModelItem::~NixTreeModelItem() {
    qDeleteAll(children);
}


void NixTreeModelItem::appendChild(NixTreeModelItem *item) {
    children.append(item);
}


NixTreeModelItem* NixTreeModelItem::child(int row) {
    if (row < children.size()) {
        return children[row];
    }
    return nullptr;
}


int NixTreeModelItem::childCount() const {
    return children.size();
}


int NixTreeModelItem::columnCount() const {
    return 5;
}


QVariant NixTreeModelItem::data(int column) const {
    return item_data;
}


int NixTreeModelItem::row() const {
    if (parent_item) {
       return parent_item->children.indexOf(const_cast<NixTreeModelItem*>(this));
    }
    return 0;
}


NixTreeModelItem* NixTreeModelItem::parentItem() {
    return parentItem();
}

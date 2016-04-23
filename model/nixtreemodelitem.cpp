#include "nixtreemodelitem.h"
#include "common/Common.hpp"


const QVector<QString> NixTreeModelItem::columns = {MODEL_HEADER_NAME, MODEL_HEADER_NIXTYPE, MODEL_HEADER_STORAGETYPE,
                                                    MODEL_HEADER_DATATYPE, MODEL_HEADER_SHAPE, MODEL_HEADER_ID,
                                                    MODEL_HEADER_CREATEDAT, MODEL_HEADER_UPDATEDAT, MODEL_HEADER_VALUE,
                                                    MODEL_HEADER_UNCERTAINTY, MODEL_HEADER_ROOTCHILDLINK};

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
        return children.value(row);
    }
    return nullptr;
}


int NixTreeModelItem::childCount() const {
    return children.count();
}


int NixTreeModelItem::columnCount() const {
    return 2;
    //return this->columns.size();
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
    return parent_item;
}

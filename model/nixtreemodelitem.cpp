#include "nixtreemodelitem.h"
#include "common/Common.hpp"


const QVector<QString> NixTreeModelItem::columns = {MODEL_HEADER_NAME, MODEL_HEADER_NIXTYPE, MODEL_HEADER_STORAGETYPE,
                                                    MODEL_HEADER_DATATYPE, MODEL_HEADER_ID,
                                                    MODEL_HEADER_CREATEDAT, MODEL_HEADER_UPDATEDAT};

NixTreeModelItem::NixTreeModelItem(const QVariant &data, NixTreeModelItem *parent) {
    std::cerr << data.canConvert<nix::Block>() << std::endl;
    checkDataType(data);
    this->parent_item = parent;
    this->item_data = data;
}


NixTreeModelItem::~NixTreeModelItem() {
    qDeleteAll(children);
}


void NixTreeModelItem::checkDataType(const QVariant &data) {
    if (data.canConvert<nix::DataArray>()) {
        this->nix_type = NixType::NIX_DATA_ARRAY;
    } else if (data.canConvert<nix::Section>()) {
        this->nix_type = NixType::NIX_SECTION;
    } else if (data.canConvert<nix::Property>()) {
        this->nix_type = NixType::NIX_PROPERTY;
    } else if (data.canConvert<nix::Tag>()) {
        this->nix_type = NixType::NIX_TAG;
    } else if (data.canConvert<nix::MultiTag>()) {
        this->nix_type = NixType::NIX_MTAG;
    } else if (data.canConvert<nix::Block>()) {
        this->nix_type = NixType::NIX_BLOCK;
    } else if (data.canConvert<nix::Group>()) {
        this->nix_type = NixType::NIX_GROUP;
    } else if (data.canConvert<nix::Source>()) {
            this->nix_type = NixType::NIX_SOURCE;
    } else if (data.canConvert<nix::Dimension>()) {
        this->nix_type = NixType::NIX_DIMENSION;
    } else {
        this->nix_type = NixType::NIX_UNKNOWN;
    }
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
    return this->columns.size();
}


QVariant NixTreeModelItem::data(int column) const {
    if (column < this->columns.count()) {
        switch (column) {
            case 0:

                return QVariant("Name");
            case 1:
                return QVariant("Type");
            case 2:
        default:
            return QVariant();
        }
    }
    return QVariant();
}


QString NixTreeModelItem::getHeader(int column) {
    return this->columns[column];
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

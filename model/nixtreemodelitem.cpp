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
    getDates();
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
    } else if (data.canConvert<nix::Feature>()) {
      this->nix_type = NixType::NIX_FEAT;
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
                return getName();
            case 1:
                return getType();
            case 2:
                return getStoreType();
            case 3:
                return getDtype();
            case 4:
                return getId();
            case 5:
                return created_at;
            case 6:
                return updated_at;
            default:
                return QVariant();
        }
    }
    return QVariant();
}


QString NixTreeModelItem::getHeader(int column) {
    return this->columns[column];
}


NixType NixTreeModelItem::nixType() const {
    return this->nix_type;
}

QVariant &NixTreeModelItem::itemData() {
    return this->item_data;
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


QVariant NixTreeModelItem::getName() const {
    switch (nix_type) {
        case NixType::NIX_BLOCK:
            return QVariant(QString::fromStdString(item_data.value<nix::Block>().name()));
        case NixType::NIX_DATA_ARRAY:
            return QVariant(QString::fromStdString(item_data.value<nix::DataArray>().name()));
        case NixType::NIX_TAG:
            return QVariant(QString::fromStdString(item_data.value<nix::Tag>().name()));
        case NixType::NIX_MTAG:
            return QVariant(QString::fromStdString(item_data.value<nix::MultiTag>().name()));
        case NixType::NIX_FEAT:
            return QVariant(QString::fromStdString(item_data.value<nix::Feature>().data().name()));
        case NixType::NIX_GROUP:
            return QVariant(QString::fromStdString(item_data.value<nix::Group>().name()));
        case NixType::NIX_SECTION:
            return QVariant(QString::fromStdString(item_data.value<nix::Section>().name()));
        case NixType::NIX_PROPERTY:
            return QVariant(QString::fromStdString(item_data.value<nix::Property>().name()));
        case NixType::NIX_DIMENSION: {
            nix::Dimension dim = item_data.value<nix::Dimension>();
            if (dim.dimensionType() == nix::DimensionType::Sample) {
                std::string s = dim.asSampledDimension().label() ? *dim.asSampledDimension().label() : nix::util::numToStr(dim.index());
                return QVariant(QString::fromStdString(s));
            } else if (dim.dimensionType() == nix::DimensionType::Range) {
                std::string s = dim.asRangeDimension().label() ? *dim.asRangeDimension().label() : nix::util::numToStr(dim.index());
                return QVariant(QString::fromStdString(s));
            } else {
                return QVariant(dim.index());
            }
        }
        default:
            return item_data;
    }
}

QVariant NixTreeModelItem::getType() const {
    switch (nix_type) {
        case NixType::NIX_BLOCK:
            return QVariant(QString::fromStdString(item_data.value<nix::Block>().type()));
        case NixType::NIX_DATA_ARRAY:
            return QVariant(QString::fromStdString(item_data.value<nix::DataArray>().type()));
        case NixType::NIX_TAG:
            return QVariant(QString::fromStdString(item_data.value<nix::Tag>().type()));
        case NixType::NIX_MTAG:
            return QVariant(QString::fromStdString(item_data.value<nix::MultiTag>().type()));
        case NixType::NIX_FEAT:
            return QVariant(QString::fromStdString(item_data.value<nix::Feature>().data().type()));
        case NixType::NIX_GROUP:
            return QVariant(QString::fromStdString(item_data.value<nix::Group>().type()));
        case NixType::NIX_SECTION:
            return QVariant(QString::fromStdString(item_data.value<nix::Section>().type()));
        case NixType::NIX_DIMENSION: {
            std::string dt = nix::util::dimTypeToStr(item_data.value<nix::Dimension>().dimensionType());
            return QVariant(QString::fromStdString(dt));
        }
        default:
            return QVariant();
    }
}


QVariant NixTreeModelItem::getId() const {
    switch (nix_type) {
        case NixType::NIX_BLOCK:
            return QVariant(QString::fromStdString(item_data.value<nix::Block>().id()));
        case NixType::NIX_DATA_ARRAY:
            return QVariant(QString::fromStdString(item_data.value<nix::DataArray>().id()));
        case NixType::NIX_TAG:
            return QVariant(QString::fromStdString(item_data.value<nix::Tag>().id()));
        case NixType::NIX_MTAG:
            return QVariant(QString::fromStdString(item_data.value<nix::MultiTag>().id()));
        case NixType::NIX_FEAT:
            return QVariant(QString::fromStdString(item_data.value<nix::Feature>().id()));
        case NixType::NIX_GROUP:
            return QVariant(QString::fromStdString(item_data.value<nix::Group>().id()));
        case NixType::NIX_SECTION:
            return QVariant(QString::fromStdString(item_data.value<nix::Section>().id()));
        case NixType::NIX_PROPERTY:
            return QVariant(QString::fromStdString(item_data.value<nix::Property>().id()));
        default:
            return QVariant();
    }
}


QVariant NixTreeModelItem::getStoreType() const {
    switch (nix_type) {
        case NixType::NIX_BLOCK:
            return QVariant(NIX_STRING_BLOCK);
        case NixType::NIX_DATA_ARRAY:
            return QVariant(NIX_STRING_DATAARRAY);
        case NixType::NIX_TAG:
            return QVariant(NIX_STRING_TAG);
        case NixType::NIX_MTAG:
            return QVariant(NIX_STRING_MULTITAG);
        case NixType::NIX_FEAT:
            return QVariant(NIX_STRING_FEATURE);
        case NixType::NIX_GROUP:
            return QVariant(NIX_STRING_GROUP);
        case NixType::NIX_SECTION:
            return QVariant(NIX_STRING_SECTION);
        case NixType::NIX_PROPERTY:
            return QVariant(NIX_STRING_PROPERTY);
        default:
            return QVariant();
    }
}


QVariant NixTreeModelItem::getDtype() const {
    switch (nix_type) {
        case NixType::NIX_DATA_ARRAY:
            return QVariant(QString::fromStdString(nix::data_type_to_string(item_data.value<nix::DataArray>().dataType())));
        case NixType::NIX_FEAT:
            return QVariant(QString::fromStdString(nix::data_type_to_string(item_data.value<nix::Feature>().data().dataType())));
        default:
            return QVariant("n.a.");
    }
}


void NixTreeModelItem::getDates() {
    switch (nix_type) {
        case NixType::NIX_BLOCK: {
            nix::Block b = item_data.value<nix::Block>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(b.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(b.updatedAt())));
            break;
        }
        case NixType::NIX_DATA_ARRAY: {
            nix::DataArray a = item_data.value<nix::DataArray>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(a.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(a.updatedAt())));
            break;
        }
        case NixType::NIX_TAG: {
            nix::Tag t = item_data.value<nix::Tag>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(t.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(t.updatedAt())));
            break;
        }
        case NixType::NIX_MTAG: {
            nix::MultiTag mt = item_data.value<nix::MultiTag>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(mt.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(mt.updatedAt())));
            break;
        }
        case NixType::NIX_FEAT: {
            nix::Feature f = item_data.value<nix::Feature>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(f.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(f.updatedAt())));
            break;
        }
        case NixType::NIX_GROUP: {
            nix::Group g = item_data.value<nix::Group>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(g.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(g.updatedAt())));
            break;
        }
        case NixType::NIX_SECTION: {
            nix::Section s = item_data.value<nix::Section>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(s.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(s.updatedAt())));
            break;
        }
        case NixType::NIX_PROPERTY: {
            nix::Property p = item_data.value<nix::Property>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(p.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(p.updatedAt())));
            break;
        }
        case NixType::NIX_SOURCE: {
            nix::Source src = item_data.value<nix::Source>();
            created_at = QVariant(QString::fromStdString(nix::util::timeToStr(src.createdAt())));
            updated_at = QVariant(QString::fromStdString(nix::util::timeToStr(src.updatedAt())));
            break;
        }
        default:
            created_at = QVariant();
            updated_at = QVariant();
    }
}

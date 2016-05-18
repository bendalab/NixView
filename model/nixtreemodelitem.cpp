#include "nixtreemodelitem.h"
#include "common/Common.hpp"
#include "utils/entitydescriptor.h"


const QVector<QString> NixTreeModelItem::columns = {MODEL_HEADER_NAME, MODEL_HEADER_NIXTYPE, MODEL_HEADER_STORAGETYPE,
                                                    MODEL_HEADER_DATATYPE, MODEL_HEADER_ID, MODEL_HEADER_VALUE,
                                                    MODEL_HEADER_CREATEDAT, MODEL_HEADER_UPDATEDAT};

NixTreeModelItem::NixTreeModelItem(const QVariant &data, NixTreeModelItem *parent) {
    this->parent_item = parent;
    setData(data);
}


NixTreeModelItem::~NixTreeModelItem() {
    qDeleteAll(children);
}


void NixTreeModelItem::setData(const QVariant &data) {
    this->item_data = data;
    this->dtype = QVariant("n.a.");
    this->value = QVariant();
    if (data.canConvert<nix::DataArray>()) {
        nix::DataArray da = data.value<nix::DataArray>();
        this->name = QVariant(da.name().c_str());
        this->type = QVariant(da.type().c_str());
        this->id = QVariant(da.id().c_str());
        this->store_type = QVariant(NIX_STRING_DATAARRAY);
        this->dtype = QVariant(nix::data_type_to_string(item_data.value<nix::DataArray>().dataType()).c_str());
        this->created_at = QVariant(nix::util::timeToStr(da.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(da.updatedAt()).c_str());
        this->nix_type = NixType::NIX_DATA_ARRAY;
    } else if (data.canConvert<nix::Section>()) {
        nix::Section s = data.value<nix::Section>();
        this->name = QVariant(s.name().c_str());
        this->type = QVariant(s.type().c_str());
        this->id = QVariant(s.id().c_str());
        this->store_type = QVariant(NIX_STRING_SECTION);
        this->created_at = QVariant(nix::util::timeToStr(s.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(s.updatedAt()).c_str());
        this->nix_type = NixType::NIX_SECTION;
    } else if (data.canConvert<nix::Property>()) {
        nix::Property p = data.value<nix::Property>();
        this->name = QVariant(p.name().c_str());
        this->type = QVariant();
        this->id = QVariant(p.id().c_str());
        this->store_type = QVariant(NIX_STRING_PROPERTY);
        this->value = getValue(p);
        this->dtype = QVariant(nix::data_type_to_string(data.value<nix::Property>().dataType()).c_str());
        this->created_at = QVariant(nix::util::timeToStr(p.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(p.updatedAt()).c_str());
        this->nix_type = NixType::NIX_PROPERTY;
    } else if (data.canConvert<nix::Tag>()) {
        nix::Tag t = data.value<nix::Tag>();
        this->name = QVariant(t.name().c_str());
        this->type = QVariant(t.type().c_str());
        this->id = QVariant(t.id().c_str());
        this->store_type = QVariant(NIX_STRING_TAG);
        this->created_at = QVariant(nix::util::timeToStr(t.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(t.updatedAt()).c_str());
        this->nix_type = NixType::NIX_TAG;
    } else if (data.canConvert<nix::MultiTag>()) {
        nix::MultiTag t = data.value<nix::MultiTag>();
        this->name = QVariant(t.name().c_str());
        this->type = QVariant(t.type().c_str());
        this->id = QVariant(t.id().c_str());
        this->store_type = QVariant(NIX_STRING_MULTITAG);
        this->created_at = QVariant(nix::util::timeToStr(t.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(t.updatedAt()).c_str());
        this->nix_type = NixType::NIX_MTAG;
    } else if (data.canConvert<nix::Block>()) {
        nix::Block b = data.value<nix::Block>();
        this->name = QVariant(b.name().c_str());
        this->type = QVariant(b.type().c_str());
        this->id = QVariant(b.id().c_str());
        this->store_type = QVariant(NIX_STRING_BLOCK);
        this->created_at = QVariant(nix::util::timeToStr(b.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(b.updatedAt()).c_str());
        this->nix_type = NixType::NIX_BLOCK;
    } else if (data.canConvert<nix::Group>()) {
        nix::Group g = data.value<nix::Group>();
        this->name = QVariant(g.name().c_str());
        this->type = QVariant(g.type().c_str());
        this->id = QVariant(g.id().c_str());
        this->store_type = QVariant(NIX_STRING_GROUP);
        this->created_at = QVariant(nix::util::timeToStr(g.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(g.updatedAt()).c_str());
        this->nix_type = NixType::NIX_GROUP;
    } else if (data.canConvert<nix::Source>()) {
        nix::Source s = data.value<nix::Source>();
        this->name = QVariant(s.name().c_str());
        this->type = QVariant(s.type().c_str());
        this->id = QVariant(s.id().c_str());
        this->store_type = QVariant(NIX_STRING_GROUP);
        this->created_at = QVariant(nix::util::timeToStr(s.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(s.updatedAt()).c_str());
        this->nix_type = NixType::NIX_SOURCE;
    } else if (data.canConvert<nix::Feature>()) {
        nix::Feature f = data.value<nix::Feature>();
        this->name = QVariant(f.data().name().c_str());
        this->type = QVariant(f.data().type().c_str());
        this->id = QVariant(f.id().c_str());
        this->dtype = QVariant(nix::data_type_to_string(f.data().dataType()).c_str());
        this->store_type = QVariant(NIX_STRING_FEATURE);
        this->created_at = QVariant(nix::util::timeToStr(f.createdAt()).c_str());
        this->updated_at = QVariant(nix::util::timeToStr(f.updatedAt()).c_str());
      this->nix_type = NixType::NIX_FEAT;
    } else if (data.canConvert<nix::Dimension>()) {
        nix::Dimension dim = data.value<nix::Dimension>();
        this->id = QVariant();
        this->created_at = QVariant();
        this->updated_at = QVariant();
        this->type = QVariant(nix::util::dimTypeToStr(dim.dimensionType()).c_str());
        this->store_type = QVariant(NIX_STRING_DIMENSION);
        if (dim.dimensionType() == nix::DimensionType::Sample) {
            std::string s = dim.asSampledDimension().label() ? *dim.asSampledDimension().label() : nix::util::numToStr(dim.index());
            this->name = QVariant(s.c_str());
        } else if (dim.dimensionType() == nix::DimensionType::Range) {
            std::string s = dim.asRangeDimension().label() ? *dim.asRangeDimension().label() : nix::util::numToStr(dim.index());
            this->name = QVariant(s.c_str());
        } else {
            this->name = QVariant(dim.index());
        }
        this->nix_type = NixType::NIX_DIMENSION;
    } else {
        this->name = data;
        this->type = QVariant();
        this->store_type = QVariant();
        this->id = QVariant();
        this->created_at = QVariant();
        this->updated_at = QVariant();
        this->nix_type = NixType::NIX_UNKNOWN;
    }
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
                return name;
            case 1:
                return type;
            case 2:
                return store_type;
            case 3:
                return dtype;
            case 4:
                return id;
            case 5:
                return value;
            case 6:
                return created_at;
            case 7:
                return updated_at;
            default:
                return QVariant();
        }
    }
    return QVariant();
}


QVariant NixTreeModelItem::toolTip() const {
    switch (nix_type) {
        case NixType::NIX_BLOCK: {
            nix::Block b = item_data.value<nix::Block>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(b)));
        }
        case NixType::NIX_DATA_ARRAY: {
            nix::DataArray da = item_data.value<nix::DataArray>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(da)));
        }
        case NixType::NIX_TAG: {
            nix::Tag t = item_data.value<nix::Tag>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(t)));
        }
        case NixType::NIX_MTAG: {
            nix::MultiTag mtag = item_data.value<nix::MultiTag>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(mtag)));
        }
        case NixType::NIX_FEAT: {
            nix::Feature f = item_data.value<nix::Feature>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(f)));
        }
        case NixType::NIX_GROUP: {
            nix::Group g = item_data.value<nix::Group>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(g)));
        }
        case NixType::NIX_SOURCE: {
            nix::Source s = item_data.value<nix::Source>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(s)));
        }
        case NixType::NIX_SECTION: {
            nix::Section s = item_data.value<nix::Section>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(s)));
        }
        case NixType::NIX_PROPERTY: {
            nix::Property p = item_data.value<nix::Property>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(p)));
        }
        case NixType::NIX_DIMENSION: {
            nix::Dimension dim = item_data.value<nix::Dimension>();
            return QVariant(QString::fromStdString(EntityDescriptor::describe(dim)));
        }
        default:
            return item_data;
    }
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
        case NixType::NIX_SOURCE:
            return QVariant(QString::fromStdString(item_data.value<nix::Source>().name()));
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
        case NixType::NIX_SOURCE:
            return QVariant(QString::fromStdString(item_data.value<nix::Source>().type()));
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
        case NixType::NIX_SOURCE:
            return QVariant(QString::fromStdString(item_data.value<nix::Source>().id()));
        default:
            return QVariant();
    }
}


QVariant NixTreeModelItem::getValue(const nix::Property &p) {
    std::string vals;
    if (p.valueCount() > 1) {
        vals = "[ ";
    }
    for (nix::Value v : p.values()) {
        vals = vals + EntityDescriptor::value_to_str(v, p.dataType());
    }
    if (p.valueCount() > 1) {
        vals = vals + "]";
    }
    return QVariant(vals.c_str());
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
        case NixType::NIX_DIMENSION:
            return QVariant(NIX_STRING_DIMENSION);
        case NixType::NIX_SOURCE:
            return QVariant(NIX_STRING_SOURCE);
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

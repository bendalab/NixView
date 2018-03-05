#include "tagcontainer.h"
#include "common/Common.hpp"
#include "plotter/plotter.h"
#include "plotter/lineplotter.h"
#include "plotter/plotwidget.h"

TagContainer::TagContainer(QVariant entity) {
    if (entity.canConvert<nix::Tag>() ||entity.canConvert<nix::MultiTag>()) {
        this->entity = entity;
    } else {
        std::cerr << "TagContainer() - Exeption: None tag entity in TagContainer." << std::endl;
        this->entity = entity;
    }
}
TagContainer::TagContainer(){}

std::vector<nix::Feature> TagContainer::features(){
    std::vector<nix::Feature> vec;
    if (entity.canConvert<nix::Tag>()) {
        vec = entity.value<nix::Tag>().features();
    } else if (entity.canConvert<nix::MultiTag>()) {
        vec = entity.value<nix::MultiTag>().features();
    }
    return vec;
}


std::vector<nix::DataArray> TagContainer::references() {
    std::vector<nix::DataArray> vec;
    if (entity.canConvert<nix::Tag>()) {
        vec = entity.value<nix::Tag>().references();
    } else if (entity.canConvert<nix::MultiTag>()) {
        vec = entity.value<nix::MultiTag>().references();
    }
    return vec;
}


QVector<QVector<double>> TagContainer::positions() {
    QVector<QVector<double>> positions;
    if (entity.canConvert<nix::Tag>()) {
        std::vector<double> pos = entity.value<nix::Tag>().position();
        positions.append(QVector<double>::fromStdVector(pos));

    } else if (entity.canConvert<nix::MultiTag>()) {
        nix::DataArray array = entity.value<nix::MultiTag>().positions();

        //USE ARRAY_TO_QVECTOR FROM PLOTTER.H (fix it first)

//        if(array.dimensionCount() == 1) {
//            nix::Dimension d = array.getDimension(1);
//            if(d.dimensionType() == nix::DimensionType::Range) {
//                QVector<double> pos = QVector<double>::fromStdVector(d.asRangeDimension().axis(array.dataExtent()[0]));
//                return positions.append(pos);
//            } else if(d.dimensionType() == nix::DimensionType::Set) {
//                QVector<double> pos = QVector<double>::fromStdVector(d.asSetDimension().)
//            }
//        } else if (array.dimensionCount() == 2) {

//        } else {
//            std::cerr << "TagContainer::positions cannot handle more than 2 dimensions." << std::endl;
//            return positions;
//        }

//        nix::DataArray array = entity.value<nix::MultiTag>().positions();
//        if(array.dataExtent().size() == 1) {
//            std::vector<double> pos = std::vector<double>(array.dataExtent()[0]);
//            array.getDataDirect(array.dataType(), pos.data(), {array.dataExtent()[0]}, {0});
//            positions.append(QVector<double>::fromStdVector(pos));
//        } else if(array.dataExtent().size() == 2 ) {
//            for (unsigned int i = 0; i < array.dataExtent()[0]; i++) {
//                std::vector<double> pos = std::vector<double>(array.dataExtent()[1]);
//                array.getDataDirect(array.dataType(), pos.data(), {array.dataExtent()[1]}, {(int) i, 0});
//                positions.append(QVector<double>::fromStdVector(pos));
//            }
//        } else {
//            std::cerr << "TagContainer::positions cannot handle more than 2 dimensions." << std::endl;
//        }

    }
    return positions;
}

//Also use plotter.h array_to_qvector ?
QVector<QVector<double>> TagContainer::extents() {
    QVector<QVector<double>> extents;
    if(this->hasExtents()) {
        if (entity.canConvert<nix::Tag>()) {
            std::vector<double> ext = entity.value<nix::Tag>().extent();
            extents.append(QVector<double>::fromStdVector(ext));

        } else if (entity.canConvert<nix::MultiTag>()) {
            nix::DataArray array = entity.value<nix::MultiTag>().extents();
            if(array.dataExtent().size() == 1) {
                std::vector<double> ext = std::vector<double>(array.dataExtent()[0]);
                array.getDataDirect(array.dataType(), ext.data(), {array.dataExtent()[0]}, {0});
                extents.append(QVector<double>::fromStdVector(ext));
            } else if(array.dataExtent().size() == 2 ) {
                for (unsigned int i = 0; i < array.dataExtent()[0]; i++) {
                    std::vector<double> ext = std::vector<double>(array.dataExtent()[1]);
                    array.getDataDirect(array.dataType(), ext.data(), {array.dataExtent()[1]}, {(int) i, 0});
                    extents.append(QVector<double>::fromStdVector(ext));
                }
            } else {
                std::cerr << "TagContainer::extents cannot handle more than 2 dimensions." << std::endl;
            }
        }
    }
    return extents;
}

bool TagContainer::hasExtents() {
    if (entity.canConvert<nix::Tag>()) {
        return (entity.value<nix::Tag>().extent().size() > 0);
    } else if (entity.canConvert<nix::MultiTag>()) {
        return (entity.value<nix::MultiTag>().extents() != nix::none);
    }
    return false;
}


std::string TagContainer::name() {
    std::string name;
    if (entity.canConvert<nix::Tag>()) {
        name = entity.value<nix::Tag>().name();
    } else if (entity.canConvert<nix::MultiTag>()) {
        name = entity.value<nix::MultiTag>().name();
    }
    return name;
}


std::string TagContainer::type() {
    std::string type;
    if (entity.canConvert<nix::Tag>()) {
        type = entity.value<nix::Tag>().type();
    } else if (entity.canConvert<nix::MultiTag>()) {
        type = entity.value<nix::MultiTag>().type();
    }
    return type;
}


std::string TagContainer::description() {
    std::string description;
    if (entity.canConvert<nix::Tag>()) {
        description = EntityDescriptor::describe(entity.value<nix::Tag>());
    } else if (entity.canConvert<nix::MultiTag>()) {
        description = EntityDescriptor::describe(entity.value<nix::MultiTag>());
    }
    return description;
}

/**
 * returns a nested QVector<QVector<QString>> to be able to describe everything of the tag in the plotter.
 *
 * ********unfinished********what else is needed
 *
 * index: 0 = normal description of the tag: name, type, description.
 * index: 1 = referenced array labels
 *
 * @brief TagContainer::completeDescription returns
 * @return
 */
QVector<QVector<QString>> TagContainer::completeDescription() {
    QVector<QVector<QString>> desc;

    QVector<QString> tagDesc;
    tagDesc.append(QString::fromStdString(this->name()));
    tagDesc.append(QString::fromStdString(this->type()));
    tagDesc.append(QString::fromStdString(this->description()));

    desc.append(tagDesc);

    if (entity.canConvert<nix::Tag>()) {

    } else if (entity.canConvert<nix::MultiTag>()) {

    }

    return desc;

}

QVariant TagContainer::getEntity() {
    return this->entity;
}

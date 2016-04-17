#include "nixarraytablemodel.h"

NixArrayTableModel::NixArrayTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

void NixArrayTableModel::set_source(const nix::DataArray &array) {
    this->array = array;
    shape = this->array.dataExtent();
    cols = shape.size() > 1 ?  shape[1] : 1;
    rows = shape[0];
    this->insertColumns(0, cols);
    this->insertRows(0, rows);
}


QVariant NixArrayTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    //orientation == Qt::Orientation::Horizontal
    if (orientation == Qt::Orientation::Horizontal) {
        if (shape.size() == 1) {
            return QString::fromStdString((array.label() ? *array.label() : "") +
                                          (array.unit() ? " [" + *array.unit() + "]" : ""));
        } else {
            if (array.getDimension(2).dimensionType() == nix::DimensionType::Set) {
                nix::SetDimension dim = array.getDimension(2).asSetDimension();
                std::vector<std::string> labels = dim.labels();
                if (section > labels.size()) {
                    return QString::fromStdString(nix::util::numToStr(section));
;               }
                return QString::fromStdString(labels[section]);
            } else if (array.getDimension(2).dimensionType() == nix::DimensionType::Range) {
                nix::RangeDimension dim = array.getDimension(2).asRangeDimension();
                return QString::fromStdString(nix::util::numToStr(dim[section]));
            } else if (array.getDimension(2).dimensionType() == nix::DimensionType::Sample) {
                nix::SampledDimension dim = array.getDimension(2).asSampledDimension();
                return dim[section];
            }
        }
    } else if (orientation == Qt::Orientation::Vertical) {
        return "1";

    }


    std::cerr << "ping3" << std::endl;
    return "1";
}

int NixArrayTableModel::rowCount(const QModelIndex &parent) const
{
    return rows;
}

int NixArrayTableModel::columnCount(const QModelIndex &parent) const
{
    return cols;
}

QVariant NixArrayTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    double d = 0.0;
    nix::NDSize count(shape.size(), 1);
    if ((index.row() < rows) && (index.column() < cols)) {
        nix::NDSize offset(shape.size(), index.row());
        if (shape.size() > 1) {
            offset[1] = index.column();
        }
        array.getData(nix::DataType::Double, &d, count, offset);
        return QVariant(d);
    }
    return QVariant();
}

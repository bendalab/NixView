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
    if (array.getDimension(1).dimensionType() == nix::DimensionType::Set) {
        v_labels = array.getDimension(1).asSetDimension().labels();
    }
    if (shape.size() > 1) {
        if (array.getDimension(2).dimensionType() == nix::DimensionType::Set) {
            h_labels = array.getDimension(2).asSetDimension().labels();
        }
    }
}


QVariant NixArrayTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Orientation::Horizontal) {
        if (shape.size() == 1) {
            return QString::fromStdString((array.label() ? *array.label() : "") +
                                          (array.unit() ? " [" + *array.unit() + "]" : ""));
        } else {
            nix::Dimension dim = array.getDimension(2);
            if (dim.dimensionType() == nix::DimensionType::Set) {
                return (size_t)section < h_labels.size() ? QString::fromStdString(h_labels[section]) :
                                                           QString::fromStdString(nix::util::numToStr(section));
            } else if (dim.dimensionType() == nix::DimensionType::Range) {
                return QString::fromStdString(nix::util::numToStr(dim.asRangeDimension()[section]));
            } else if (dim.dimensionType() == nix::DimensionType::Sample) {
                return dim.asSampledDimension()[section];
            }
        }
    } else if (orientation == Qt::Orientation::Vertical) {
        nix::Dimension dim = array.getDimension(1);
        if (dim.dimensionType() == nix::DimensionType::Set) {
            return (size_t)section < v_labels.size() ? QString::fromStdString(v_labels[section]) :
                                                       QString::fromStdString(nix::util::numToStr(section));
        } else if (dim.dimensionType() == nix::DimensionType::Sample) {
            return dim.asSampledDimension()[section];
        } else if (dim.dimensionType() == nix::DimensionType::Range) {
            return dim.asRangeDimension()[section];
        }
        return "1";
    }
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
    if (!index.isValid() || role != Qt::DisplayRole) {
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

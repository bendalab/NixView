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
    //orientation == Qt::Orientation::Horizontal
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

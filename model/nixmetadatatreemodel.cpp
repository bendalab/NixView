#include "nixmetadatatreemodel.h"

NixMetadataTreeModel::NixMetadataTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant NixMetadataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex NixMetadataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex NixMetadataTreeModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int NixMetadataTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int NixMetadataTreeModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant NixMetadataTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

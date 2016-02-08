#include "NixProxyModel.hpp"

NixProxyModel::NixProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    setFilterRegExp(QRegExp("*fish*"));
}

bool NixProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    NixDataModel *model = static_cast<NixDataModel*>(sourceModel());
    for (int i = 0; i < model->num_columns; ++i)
    {
        QModelIndex index = model->index(source_row, i, source_parent);
        if (model->data(index).toString().contains(filterRegExp()))
            return true;
    }
    return false;
}



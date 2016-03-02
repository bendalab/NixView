#include "NixProxyModel.hpp"
#include <qdebug.h>

NixProxyModel::NixProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    setFilterRegExp(QRegExp("source"));
    set_filter_mode(3);
}

bool NixProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(filter_mode == 1)
    {
        if(check_children(source_row, source_parent))
            return true;
    }

    else if(filter_mode == 2)
    {
        if (check_parent(source_parent))
            return true;
    }

    else if(filter_mode == 3)
    {
        if (check_parent(source_parent) || check_children(source_row, source_parent))
            return true;
    }

    return check_entry_row(source_row, source_parent);
}


bool NixProxyModel::check_parent(const QModelIndex &source_parent) const
{
    QModelIndex parent = source_parent;
    while (parent.isValid()) {
        if (check_entry_row(parent.row(), parent.parent()))
            return true;
        parent = parent.parent();
    }
    return false;
}

bool NixProxyModel::check_children(int source_row, const QModelIndex &source_parent) const
{
    // get source-model index for current row
    QModelIndex current_item = sourceModel()->index(source_row, 0, source_parent);
    if(current_item.isValid())
    {
        // if any of children matches the filter, then current index matches the filter as well
        int num_children = sourceModel()->rowCount(current_item) ;
        for(int i = 0; i<num_children; ++i)
        {
            if(filterAcceptsRow(i, current_item))
            {
                return true ;
            }
        }
    }
    return false;
}

bool NixProxyModel::check_entry_row(int source_row, const QModelIndex &source_parent) const
{
    NixDataModel *model = static_cast<NixDataModel*>(sourceModel());

    for (int c = 0; c < model->num_columns; ++c)
    {
        QModelIndex index = model->index(source_row, c, source_parent);
        if (model->data(index).toString().contains(filterRegExp()))
            return true;
    }
    return false;
}


#include "NixProxyModel.hpp"
#include <qdebug.h>
#include "common/Common.hpp"

NixProxyModel::NixProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    fine_filter = "";
    rough_filter = FILTER_EXP_NONE;
}

bool NixProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(block_mode)
        return false;

    if(metadata_only_mode)
        if(check_if_in_data_branch(source_row, source_parent))
            return false;

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

    bool rough_filter_satisfied;
    (strcmp(rough_filter.toStdString().c_str(), FILTER_EXP_NONE) == 0 || strcmp(rough_filter.toStdString().c_str(), FILTER_EXP_METADATA) == 0)?
                rough_filter_satisfied = true:
                rough_filter_satisfied = false;

    // DataArray check
    if (strcmp(rough_filter.toStdString().c_str(), FILTER_EXP_DATAARRAY) == 0)
    {
        int c = 2; //Storage Type, see NixDataModel.cpp
        QModelIndex index = model->index(source_row, c, source_parent);
        if (strcmp(model->data(index).toString().toStdString().c_str(), NIX_STRING_DATAARRAY) == 0)
            rough_filter_satisfied = true;
    }

    // NameContains check
    if (strcmp(rough_filter.toStdString().c_str(), FILTER_EXP_NAME) == 0)
    {
        QModelIndex index = model->index(source_row, 0, source_parent);
        return model->data(index).toString().contains(fine_filter);
    }

    // check if rough filter  satisfied
    if (!rough_filter_satisfied)
        return false;

    // fine filter --> check if entry row contains fine_filter experession
    for (int c = 0; c < model->num_columns; ++c)
    {
        QModelIndex index = model->index(source_row, c, source_parent);
        if (model->data(index).toString().contains(fine_filter))
            return true;
    }
    return false;
}

bool NixProxyModel::check_if_in_data_branch(int, const QModelIndex &source_parent) const
{
    QModelIndex parent = source_parent;
    while (parent.isValid()) {
        if (!parent.parent().isValid() && strcmp(sourceModel()->data(parent).toString().toStdString().c_str(), "Data") == 0)
            return true;
        parent = parent.parent();
    }
    return false;
}

NixModelItem* NixProxyModel::get_item_from_qml(QModelIndex qml)
{
    return static_cast<NixModelItem*>(static_cast<NixDataModel*>(sourceModel())->itemFromIndex(qml));
}

void NixProxyModel::set_rough_filter(QString exp)
{
    rough_filter = exp;
    qDebug() << exp;

    (strcmp(rough_filter.toStdString().c_str(), FILTER_EXP_METADATA) == 0)?
                metadata_only_mode = true:
                metadata_only_mode = false;

    refresh();
}

void NixProxyModel::set_fine_filter(QString exp)
{
    fine_filter = exp;
    qDebug() << exp;
    refresh();
}

void NixProxyModel::refresh()
{
    invalidateFilter();
}


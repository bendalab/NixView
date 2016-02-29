#include "NixProxyModel.hpp"
#include <qdebug.h>

NixProxyModel::NixProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    setFilterRegExp(QRegExp("data"));
    setFilterKeyColumn(0);
}

bool NixProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
//    NixDataModel *model = static_cast<NixDataModel*>(sourceModel());
//    for (int i = 0; i < model->num_columns; ++i)
//    {
//        QModelIndex index = model->index(source_row, i, source_parent);
//        if (model->data(index).toString().contains(filterRegExp()))
//            return true;
//    }
//    return false;

    // get source-model index for current row
    QModelIndex parent_index = sourceModel()->index(source_row, this->filterKeyColumn(), source_parent);
    if(parent_index.isValid())
    {
        // if any of children matches the filter, then current index matches the filter as well
        int i, nb = sourceModel()->rowCount(parent_index) ;
        for(i=0; i<nb; ++i)
        {
            if(filterAcceptsRow(i, parent_index))
            {
                return true ;
            }
        }
        // check current index itself :
        QString key = sourceModel()->data(parent_index, filterRole()).toString();
        return key.contains(filterRegExp()) ;
    }


//    qDebug() << source_row << " " << source_parent;
//    return true;

//    return false;
}



#ifndef NIXPROXYMODEL_H
#define NIXPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <model/NixDataModel.hpp>

class NixProxyModel : public QSortFilterProxyModel
{
public:
    NixProxyModel(QObject * parent = 0);

    std::string current_keyword;

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

};

#endif // NIXPROXYMODEL_H

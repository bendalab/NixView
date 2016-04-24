#ifndef NIXTREEMODEL_H
#define NIXTREEMODEL_H

#include <QAbstractItemModel>
#include <nix.hpp>
#include "nixtreemodelitem.h"


class NixTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NixTreeModel(QObject *parent = 0);

    ~NixTreeModel();

    void set_entity(const nix::File &nixfile);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
};

#endif // NIXTREEMODEL_H

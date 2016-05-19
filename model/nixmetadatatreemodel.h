#ifndef NIXMETADATATREEMODEL_H
#define NIXMETADATATREEMODEL_H

#include <QAbstractItemModel>
#include <nix.hpp>
#include "nixtreemodelitem.h"


class NixMetadataTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NixMetadataTreeModel(QObject *parent = 0);
    ~NixMetadataTreeModel();

    void setEntity(const nix::Section &section);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    NixTreeModelItem *rootItem;
    int checkForKids(NixTreeModelItem *item) const;
    void fetchSection(const nix::Section &s, NixTreeModelItem *parent);
    void appendSections(const std::vector<nix::Section> &sections, NixTreeModelItem *parent);
    void appendProperties(const std::vector<nix::Property> &properties, NixTreeModelItem *parent);

};

#endif // NIXMETADATATREEMODEL_H

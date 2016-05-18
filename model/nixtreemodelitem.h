#ifndef NIXTREEMODELITEM_H
#define NIXTREEMODELITEM_H
#include <QVariant>
#include <QList>
#include <QVector>
#include <nix.hpp>


enum class NixType {
    NIX_BLOCK,
    NIX_DATA_ARRAY,
    NIX_TAG,
    NIX_MTAG,
    NIX_GROUP,
    NIX_FEAT,
    NIX_SOURCE,
    NIX_SECTION,
    NIX_PROPERTY,
    NIX_DIMENSION,
    NIX_UNKNOWN
};


class NixTreeModelItem {

public:
    explicit NixTreeModelItem(const QVariant &data, NixTreeModelItem *parentItem = 0);
    ~NixTreeModelItem();

    static const QVector<QString> columns;

    void appendChild(NixTreeModelItem *child);
    NixTreeModelItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    NixTreeModelItem *parentItem();
    QString getHeader(int column);
    NixType nixType() const;
    QVariant& itemData();
    QVariant toolTip() const;

private:
    QList<NixTreeModelItem*> children;
    QVariant item_data;
    NixTreeModelItem *parent_item;
    NixType nix_type;
    QVariant created_at, updated_at, name, dtype, id, store_type, type, value;

    void setData(const QVariant &data);
    QVariant getValue(const nix::Property &p);
};

#endif // NIXTREEMODELITEM_H

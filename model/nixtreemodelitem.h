#ifndef NIXTREEMODELITEM_H
#define NIXTREEMODELITEM_H
#include <QVariant>
#include <QList>
#include <QVector>


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

    void appendChild(NixTreeModelItem *child);

    NixTreeModelItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    NixTreeModelItem *parentItem();
    QString getHeader(int column);
    NixType nixType() const;

private:
    QList<NixTreeModelItem*> children;
    QVariant item_data;
    NixTreeModelItem *parent_item;
    NixType nix_type;
    QVariant created_at, updated_at;
    static const QVector<QString> columns;

    void checkDataType(const QVariant &data);
    QVariant getName() const;
    QVariant getType() const;
    QVariant getId() const;
    QVariant getStoreType() const;
    QVariant getDtype() const;
    void getDates();
};

#endif // NIXTREEMODELITEM_H

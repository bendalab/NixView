#ifndef NIXTREEMODELITEM_H
#define NIXTREEMODELITEM_H
#include <QVariant>
#include <QList>
#include <QVector>

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

private:
    QList<NixTreeModelItem*> children;
    QVariant item_data;
    NixTreeModelItem *parent_item;
    static const QVector<QString> columns;
};

#endif // NIXTREEMODELITEM_H

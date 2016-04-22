#ifndef NIXTREEMODELITEM_H
#define NIXTREEMODELITEM_H
#include <QVariant>
#include <QList>
#include "common/Common.hpp"

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

private:
    QList<NixTreeModelItem*> children;
    QVariant item_data;
    NixTreeModelItem *parent_item;
};
// http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
#endif // NIXTREEMODELITEM_H

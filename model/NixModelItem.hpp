#ifndef NIXMODELITEM_H
#define NIXMODELITEM_H

#include <QStandardItem>

class NixModelItem : public QStandardItem
{
public:
    NixModelItem();
    NixModelItem(const QString & text);
};

#endif // NIXMODELITEM_H

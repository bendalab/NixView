#ifndef NIXMODELITEM_H
#define NIXMODELITEM_H

#include <QStandardItem>
#include <QVariant>

class NixModelItem : public QStandardItem
{
public:
    NixModelItem();
    NixModelItem(const QString & text);
    NixModelItem(const QString & text, QVariant _nix_entity);

private:
    QVariant nix_entity;
    std::string nix_entity_type;
};

#endif // NIXMODELITEM_H

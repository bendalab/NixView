#ifndef NIXMODELITEM_H
#define NIXMODELITEM_H

#include <QStandardItem>
#include <QVariant>

//TODO make it template class??

class NixModelItem : public QStandardItem
{
public:
    NixModelItem();
    NixModelItem(const QString & text);

private:
    QVariant nix_entity;
    std::string nix_entity_type;
};

#endif // NIXMODELITEM_H

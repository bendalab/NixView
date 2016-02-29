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

    // getter
    const std::string get_nix_qvariant_type() const { return nix_entity_type; }
    template <typename T>
    const T get_nix_entity() const { if(nix_entity.canConvert<T>()) return nix_entity.value<T>(); }

private:
    QVariant nix_entity;
    std::string nix_entity_type;
};

#endif // NIXMODELITEM_H

#ifndef NIXMODELITEM_H
#define NIXMODELITEM_H

#include <QStandardItem>
#include <QVariant>
#include <nix.hpp>


class NixModelItem : public QStandardItem
{
public:
    NixModelItem();
    NixModelItem(const QString & text);
    NixModelItem(const QString & text, QVariant _nix_entity);

    nix::Section get_entity_metadata();

    // getter
    const std::string get_nix_qvariant_type() const { return nix_entity_type; }
    template <typename T>
    const T get_nix_entity() const { return nix_entity.value<T>(); }
    const bool entity_can_have_metadata() const { return entity_metadata; }

private:
    QVariant nix_entity;
    std::string nix_entity_type;
    bool entity_metadata;
};

#endif // NIXMODELITEM_H

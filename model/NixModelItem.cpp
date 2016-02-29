#include "NixModelItem.hpp"
#include <common/Common.hpp>

NixModelItem::NixModelItem()
    : QStandardItem()
{
    nix_entity_type = "";
}

NixModelItem::NixModelItem(const QString &text)
    : QStandardItem(text)
{
    nix_entity_type = "";
}

NixModelItem::NixModelItem(const QString &text, QVariant _nix_entity)
    : QStandardItem(text)
{
    nix_entity = _nix_entity;

    if(nix_entity.canConvert<nix::Block>())
        nix_entity_type = "Block";
    else if(nix_entity.canConvert<nix::Group>())
        nix_entity_type = "Group";
    else if(nix_entity.canConvert<nix::DataArray>())
        nix_entity_type = "DataArray";
    else if(nix_entity.canConvert<nix::MultiTag>())
        nix_entity_type = "MultiTag";
    else if(nix_entity.canConvert<nix::Tag>())
        nix_entity_type = "Tag";
    else if(nix_entity.canConvert<nix::Section>())
        nix_entity_type = "Section";
    else if(nix_entity.canConvert<nix::Source>())
        nix_entity_type = "Source";
    else if(nix_entity.canConvert<nix::Property>())
        nix_entity_type = "Property";
}

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
        nix_entity_type = NIX_STRING_BLOCK;
    else if(nix_entity.canConvert<nix::Group>())
        nix_entity_type = NIX_STRING_GROUP;
    else if(nix_entity.canConvert<nix::DataArray>())
        nix_entity_type = NIX_STRING_DATAARRAY;
    else if(nix_entity.canConvert<nix::MultiTag>())
        nix_entity_type = NIX_STRING_MULTITAG;
    else if(nix_entity.canConvert<nix::Tag>())
        nix_entity_type = NIX_STRING_TAG;
    else if(nix_entity.canConvert<nix::Section>())
        nix_entity_type = NIX_STRING_SECTION;
    else if(nix_entity.canConvert<nix::Source>())
        nix_entity_type = NIX_STRING_SOURCE;
    else if(nix_entity.canConvert<nix::Property>())
        nix_entity_type = NIX_STRING_PROPERTY;
}

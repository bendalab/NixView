#include "NixModelItem.hpp"
#include <common/Common.hpp>

NixModelItem::NixModelItem()
    : QStandardItem()
{
    nix_entity_type = "";
    entity_metadata = false;
}

NixModelItem::NixModelItem(const QString &text)
    : QStandardItem(text)
{
    nix_entity_type = "";
    entity_metadata = false;
}

NixModelItem::NixModelItem(const QString &text, QVariant _nix_entity)
    : QStandardItem(text)
{
    nix_entity = _nix_entity;

    if(nix_entity.canConvert<nix::Block>())
    {
        nix_entity_type = NIX_STRING_BLOCK;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::Group>())
    {
        nix_entity_type = NIX_STRING_GROUP;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::DataArray>())
    {
        nix_entity_type = NIX_STRING_DATAARRAY;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::MultiTag>())
    {
        nix_entity_type = NIX_STRING_MULTITAG;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::Tag>())
    {
        nix_entity_type = NIX_STRING_TAG;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::Feature>())
    {
        nix_entity_type = NIX_STRING_FEATURE;
        entity_metadata = false;
    }
    else if(nix_entity.canConvert<nix::Section>())
    {
        nix_entity_type = NIX_STRING_SECTION;
        entity_metadata = false;
    }
    else if(nix_entity.canConvert<nix::Source>())
    {
        nix_entity_type = NIX_STRING_SOURCE;
        entity_metadata = true;
    }
    else if(nix_entity.canConvert<nix::Property>())
    {
        nix_entity_type = NIX_STRING_PROPERTY;
        entity_metadata = false;
    }
}

nix::Section NixModelItem::get_entity_metadata()
{
    if(nix_entity_type == NIX_STRING_BLOCK)
        return nix_entity.value<nix::Block>().metadata();
    if(nix_entity_type == NIX_STRING_GROUP)
        return nix_entity.value<nix::Group>().metadata();
    if(nix_entity_type == NIX_STRING_DATAARRAY)
        return nix_entity.value<nix::DataArray>().metadata();
    if(nix_entity_type == NIX_STRING_MULTITAG)
        return nix_entity.value<nix::MultiTag>().metadata();
    if(nix_entity_type == NIX_STRING_TAG)
        return nix_entity.value<nix::Tag>().metadata();
    if(nix_entity_type == NIX_STRING_SOURCE)
        return nix_entity.value<nix::Source>().metadata();
    return nix::Section();
}

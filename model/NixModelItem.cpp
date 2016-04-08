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
//        nix::Block b = nix_entity.value<nix::Block>();
//        std::string tooltip = "Block: " + b.type() + " --- " + b.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Group>())
    {
        nix_entity_type = NIX_STRING_GROUP;
        entity_metadata = true;
//        nix::Group g = nix_entity.value<nix::Group>();
//        std::string tooltip = "Group: " + g.type() + " --- " + g.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::DataArray>())
    {
        nix_entity_type = NIX_STRING_DATAARRAY;
        entity_metadata = true;
//        nix::DataArray da = nix_entity.value<nix::DataArray>();
//        std::string tooltip = "DataArray: " + da.type() + " --- " + da.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::MultiTag>())
    {
        nix_entity_type = NIX_STRING_MULTITAG;
        entity_metadata = true;
//        nix::MultiTag mtag = nix_entity.value<nix::MultiTag>();
//        std::string tooltip = "MultiTag: " + mtag.type() + " --- " + mtag.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Tag>())
    {
        nix_entity_type = NIX_STRING_TAG;
        entity_metadata = true;
//        nix::Tag t = nix_entity.value<nix::Tag>();
//        std::string tooltip = "Tag: " + t.type() + " --- " + t.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Feature>())
    {
        nix_entity_type = NIX_STRING_FEATURE;
        entity_metadata = false;
//        nix::Feature f = nix_entity.value<nix::Feature>();
//        std::string tooltip = "Feature: " + f.data().type() + " --- " + f.data().type();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Section>())
    {
        nix_entity_type = NIX_STRING_SECTION;
        entity_metadata = false;
//        nix::Section s = nix_entity.value<nix::Section>();
//       std::string tooltip = "Section: " + s.type() + " --- " + s.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Source>())
    {
        nix_entity_type = NIX_STRING_SOURCE;
        entity_metadata = true;
//        nix::Source s = nix_entity.value<nix::Source>();
//        std::string tooltip = "Source: " + s.type() + " --- " + s.name();
//        setToolTip(QString::fromStdString(tooltip));
    }
    else if(nix_entity.canConvert<nix::Property>())
    {
        nix_entity_type = NIX_STRING_PROPERTY;
        entity_metadata = false;
//        nix::Property p = nix_entity.value<nix::Property>();
//        std::string tooltip = "Property: " + p.name() + " --- " + (p.unit().is_initialized() ? *p.unit() : "");
//        setToolTip(QString::fromStdString(tooltip));
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

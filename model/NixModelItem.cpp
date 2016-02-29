#include "NixModelItem.hpp"
#include <common/Common.hpp>

NixModelItem::NixModelItem()
    : QStandardItem()
{

}

NixModelItem::NixModelItem(const QString & text)
    : QStandardItem(text)
{

}

NixModelItem::NixModelItem(const QString & text, QVariant _nix_entity)
    : QStandardItem(text)
{
    nix_entity = _nix_entity;
}

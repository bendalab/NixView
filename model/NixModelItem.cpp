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

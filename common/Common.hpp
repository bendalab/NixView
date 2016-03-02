#include <nix.hpp>

Q_DECLARE_METATYPE(nix::Block);
Q_DECLARE_METATYPE(nix::Group);
Q_DECLARE_METATYPE(nix::Section);
Q_DECLARE_METATYPE(nix::Property);
Q_DECLARE_METATYPE(nix::DataArray);
Q_DECLARE_METATYPE(nix::Tag);
Q_DECLARE_METATYPE(nix::MultiTag);
Q_DECLARE_METATYPE(nix::Source);

#define NIX_STRING_BLOCK "Block"
#define NIX_STRING_GROUP "Group"
#define NIX_STRING_METADATA "MetaData"
#define NIX_STRING_SECTION "Section"
#define NIX_STRING_DATAARRAY "DataArray"
#define NIX_STRING_TAG "Tag"
#define NIX_STRING_MULTITAG "MultiTag"
#define NIX_STRING_SOURCE "Source"
#define NIX_STRING_PROPERTY "Property"

#define NIX_STRING_TYPE_STRING "String"
#define NIX_STRING_TYPE_BOOL "Bool"
#define NIX_STRING_TYPE_INT32 "Int32"
#define NIX_STRING_TYPE_INT64 "Int64"
#define NIX_STRING_TYPE_UINT64 "UInt64"
#define NIX_STRING_TYPE_DOUBLE "Double"

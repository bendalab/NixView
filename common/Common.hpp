#include <nix.hpp>

// declare NIX entitites as QVariant types
Q_DECLARE_METATYPE(nix::Block);
Q_DECLARE_METATYPE(nix::Group);
Q_DECLARE_METATYPE(nix::Section);
Q_DECLARE_METATYPE(nix::Property);
Q_DECLARE_METATYPE(nix::DataArray);
Q_DECLARE_METATYPE(nix::Tag);
Q_DECLARE_METATYPE(nix::MultiTag);
Q_DECLARE_METATYPE(nix::Feature);
Q_DECLARE_METATYPE(nix::Source);

// define strings regarding NIX for consistent use
#define NIX_STRING_BLOCK "Block"
#define NIX_STRING_GROUP "Group"
#define NIX_STRING_METADATA "MetaData"
#define NIX_STRING_SECTION "Section"
#define NIX_STRING_DATAARRAY "DataArray"
#define NIX_STRING_TAG "Tag"
#define NIX_STRING_MULTITAG "MultiTag"
#define NIX_STRING_FEATURE "Feature"
#define NIX_STRING_SOURCE "Source"
#define NIX_STRING_PROPERTY "Property"

#define NIX_STRING_TYPE_STRING "String"
#define NIX_STRING_TYPE_BOOL "Bool"
#define NIX_STRING_TYPE_INT32 "Int32"
#define NIX_STRING_TYPE_INT64 "Int64"
#define NIX_STRING_TYPE_UINT64 "UInt64"
#define NIX_STRING_TYPE_DOUBLE "Double"

#define REFERENCE_DATAARRAY "Reference/DataArray"
#define FEATURE_DATAARRAY "Feature/DataArray"

// define views as integers
#define VIEW_TREE 0
#define VIEW_COLUMN 1

// define filter expressions
#define FILTER_EXP_NONE "-"
#define FILTER_EXP_METADATA "is MetaData"
#define FILTER_EXP_DATAARRAY "is DataArray"
#define FILTER_EXP_NAME "Name contains"
//TODO more expressions

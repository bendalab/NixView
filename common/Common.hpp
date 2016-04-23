#include <nix.hpp>

// declare NIX entitites as QVariant types
Q_DECLARE_METATYPE(nix::Block)
Q_DECLARE_METATYPE(nix::Group)
Q_DECLARE_METATYPE(nix::Section)
Q_DECLARE_METATYPE(nix::Property)
Q_DECLARE_METATYPE(nix::DataArray)
Q_DECLARE_METATYPE(nix::Tag)
Q_DECLARE_METATYPE(nix::MultiTag)
Q_DECLARE_METATYPE(nix::Feature)
Q_DECLARE_METATYPE(nix::Source)
Q_DECLARE_METATYPE(nix::Dimension)

// define model headers
#define MODEL_HEADER_NAME "Name"
#define MODEL_HEADER_NIXTYPE "Nix Type"
#define MODEL_HEADER_STORAGETYPE "Storage Type"
#define MODEL_HEADER_DATATYPE "Data Type"
#define MODEL_HEADER_SHAPE "Shape"
#define MODEL_HEADER_ID "ID"
#define MODEL_HEADER_CREATEDAT "Created at"
#define MODEL_HEADER_UPDATEDAT "Updated at"
#define MODEL_HEADER_VALUE "Value"
#define MODEL_HEADER_UNCERTAINTY "Uncertainty"
#define MODEL_HEADER_ROOTCHILDLINK "root_child_link"

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
#define FILTER_EXP_GROUP "is Group"
#define FILTER_EXP_BLOCK "is Block"
#define FILTER_EXP_METADATA "is MetaData"
#define FILTER_EXP_DATAARRAY "is DataArray"
#define FILTER_EXP_TAG "is Tag"
#define FILTER_EXP_MULTITAG "is MultiTag"
#define FILTER_EXP_NAME_CONTAINS "Name contains"
#define FILTER_EXP_NIXTYPE_CONTAINS "Nix Type contains"

// define setting expressions
#define S_RAWTREEVIEW "rawtreeview"
#define S_COLUMNSHIDDEN "columnshidden"

#define RECENT_FILES_GROUP "recent_files"
#define RECENT_FILES_MAX_COUNT 5

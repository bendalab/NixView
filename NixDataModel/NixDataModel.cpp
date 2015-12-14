#include "NixDataModel.hpp"
#include <iostream>

NixDataModel::NixDataModel() :
    QStandardItemModel()
{
    QStandardItem* root_node = this->invisibleRootItem();

    QList<QString> headers;
    headers << "Name" << "Nix Type" << "Storage Type" << "Data Type" << "Shape";
    setHorizontalHeaderLabels(headers);
    QList<QStandardItem*> header_row = create_entry_row(headers);
    root_node->appendRow(header_row);
}

QList<QStandardItem*> NixDataModel::create_entry_row(QList<QString> row_entries)
{
    QList<QStandardItem*> row_items;
    for (QString entry : row_entries)
        row_items << new QStandardItem(entry);
//    row_items << new QStandardItem(QString("Test"));
    return row_items;
}

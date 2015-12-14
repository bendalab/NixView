#ifndef NIXDATAMODEL_H
#define NIXDATAMODEL_H

#include <QStandardItemModel>


class NixDataModel : public QStandardItemModel
{
public:
    NixDataModel();

private:
    QStandardItem* root_node;
    QList<QStandardItem*> create_entry_row(QList<QString>);
};

#endif // NIXDATAMODEL_H

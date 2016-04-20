#ifndef DATATABLE_H
#define DATATABLE_H

#include <QWidget>
#include <QModelIndex>
#include <nix.hpp>
#include "model/NixModelItem.hpp"
#include "model/nixarraytablemodel.h"
#include <QStandardItemModel>
#include <QStringList>
#include "utils/entitydescriptor.h"
#include <QTableView>

namespace Ui {
class DataTable;
}

class DataTable : public QWidget
{
    Q_OBJECT

public:
    explicit DataTable(QWidget *parent = 0);
    ~DataTable();

    void set_entity(const QModelIndex qml);
    bool can_draw(const QModelIndex qml) const;
    QTableView* get_table();

private:
    Ui::DataTable *ui;
    nix::DataArray array;
    NixArrayTableModel *model;

    void build_model();
    void describe();
    EntityDescriptor basic_description(const std::string &name, const std::string &type, const std::string &description,
                                       const std::string &id, const std::string &created, const std::string &updated);
};

#endif // DATATABLE_H

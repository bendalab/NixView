#ifndef DATATABLE_H
#define DATATABLE_H

#include <QWidget>
<<<<<<< HEAD
=======
#include <QModelIndex>
#include <nix.hpp>
#include "model/NixModelItem.hpp"
#include <QStandardItemModel>
>>>>>>> new data table class

namespace Ui {
class DataTable;
}

class DataTable : public QWidget
{
    Q_OBJECT

public:
    explicit DataTable(QWidget *parent = 0);
    ~DataTable();

<<<<<<< HEAD
private:
    Ui::DataTable *ui;
=======
    void set_entity(QModelIndex qml);
    bool can_draw(QModelIndex qml) const;

private:
    Ui::DataTable *ui;
    nix::DataArray array;
    QStandardItemModel *model;

    void build_model();

>>>>>>> new data table class
};

#endif // DATATABLE_H

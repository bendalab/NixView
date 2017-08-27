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

    void set_entity(const QVariant var);
    bool can_draw(const QVariant var) const;
    QTableView* get_table();
    nix::DataArray getArray();
    int currentPage();

private:
    Ui::DataTable *ui;
    nix::DataArray array;
    NixArrayTableModel *model;

    void build_model(int page = 0);

public slots:
    void select_page();
    void previous_page();
    void next_page();
};

#endif // DATATABLE_H

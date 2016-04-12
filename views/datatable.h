#ifndef DATATABLE_H
#define DATATABLE_H

#include <QWidget>

namespace Ui {
class DataTable;
}

class DataTable : public QWidget
{
    Q_OBJECT

public:
    explicit DataTable(QWidget *parent = 0);
    ~DataTable();

private:
    Ui::DataTable *ui;
};

#endif // DATATABLE_H

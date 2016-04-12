#include "tabledialog.hpp"
#include "ui_tabledialog.h"

TableDialog::TableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableDialog)
{
    ui->setupUi(this);
}

TableDialog::~TableDialog()
{
    delete ui;
}


void TableDialog::set_entity(const QModelIndex qml) {
    ui->data_table->set_entity(qml);
}

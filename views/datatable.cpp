#include "datatable.h"
#include "ui_datatable.h"

DataTable::DataTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTable)
{
    ui->setupUi(this);
}

DataTable::~DataTable()
{
    delete ui;
}

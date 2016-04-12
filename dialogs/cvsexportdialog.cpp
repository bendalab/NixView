#include "cvsexportdialog.h"
#include "ui_cvsexportdialog.h"
#include <iostream>
#include <Qt>

CVSExportDialog::CVSExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CVSExportDialog)
{
    ui->setupUi(this);
}

CVSExportDialog::~CVSExportDialog()
{
    delete ui;
}


void CVSExportDialog::set_table(QTableView *table) {
    this->table = table;
}


void CVSExportDialog::accept() {
    export_cvs();
}


void CVSExportDialog::export_cvs() {
    QModelIndexList indexes = table->selectionModel()->selection().indexes();
    int max_col = indexes.back().column();
    QString sep = ui->separator_edit->text();
    for (QModelIndex i: indexes) {
        QVariant var = table->model()->data(i);
        if (!var.canConvert<double>())
            continue;
        if (i.column() < max_col)
            std::cerr << var.value<double>() << sep.toStdString() << " ";
        else
            std::cerr << var.value<double>() << std::endl;
    }
}

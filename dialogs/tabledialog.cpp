#include "tabledialog.hpp"
#include "ui_tabledialog.h"
#include "dialogs/csvexportdialog.h"

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


void TableDialog::set_entity(const QVariant var) {
    ui->data_table->set_entity(var);
}


void TableDialog::button_clicked(QAbstractButton *button) {
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Close)) {
        this->close();
    }
}


void TableDialog::accept() {
    CSVExportDialog d(this);

    QModelIndexList indexes = ui->data_table->get_table()->selectionModel()->selection().indexes();

    if(! indexes.isEmpty()) {
        nix::NDSize start(1,0);
        nix::NDSize extend(1,0);

        int size = ui->data_table->getArray().dataExtent().size();
        if(size == 1) {
            start = {indexes[0].row()};
            extend   = {indexes.last().row()+1};
        } else if(size == 2) {
            start = {indexes[0].row(),      indexes[0].column()};
            extend   = {indexes.last().row()+1,  indexes.last().column()+1};
        } else if(size == 3) {
            start = {indexes[0].row(),      indexes[0].column(),    ui->data_table->currentPage()-1};
            extend   = {indexes.last().row()+1,  indexes.last().column()+1,ui->data_table->currentPage()};
        }

        d.setSelection(start, extend);
    }
    d.setArray(ui->data_table->getArray());
    d.exec();
}

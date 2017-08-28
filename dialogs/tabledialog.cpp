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

    nix::NDSize start(1,-1);
    nix::NDSize end(1,-1);
    if(! indexes.isEmpty()) {
        start = {indexes[0].row(),      indexes[0].column()} ;
        end   = {indexes.back().row(),  indexes.back().column()};
    }
    d.setArray(ui->data_table->getArray());
    //d.setSelection(start, end);
    d.exec();
}

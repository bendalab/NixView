#include "tabledialog.hpp"
#include "ui_tabledialog.h"
#include "dialogs/cvsexportdialog.h"

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


void TableDialog::button_clicked(QAbstractButton *button) {
    if ((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Close)) {
        this->close();
    }
}


void TableDialog::accept() {
    CVSExportDialog d(this);
    d.set_table(ui->data_table->get_table());
    d.exec();
}

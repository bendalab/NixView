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


void CVSExportDialog::get_header(QStringList &vheader, QStringList &hheader) {
    for (int i = 0; i < table->model()->columnCount(); i++)
      hheader.push_back(table->model()->headerData(i, Qt::Horizontal).toString());
    for (int i = 0; i < table->model()->rowCount(); i++)
        vheader.push_back(table->model()->headerData(i, Qt::Vertical).toString());
}


void CVSExportDialog::export_cvs() {
    QItemSelectionModel* temp = table->selectionModel();
    if (!ui->export_selection->isChecked() || temp->selection().count() == 0) {
        table->selectAll();
    }

    QModelIndexList indexes = table->selectionModel()->selection().indexes();
    int min_col = indexes[0].column();
    int max_col = indexes.back().column();
    int min_row = indexes[0].row();
    int max_row = indexes.back().row();

    QStringList vheader, hheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader);
    }

    QString sep = ui->separator_edit->text();
    std::cerr << min_col << "\t"<< max_col  << std::endl;
    std::cerr << min_row << "\t"<< max_row  << std::endl;
    if (ui->export_header->isChecked()) {
        std::cerr << " " << sep.toStdString() << " ";
        for (int i = 0; i < hheader.size(); i++) {
            if (i >= min_col && i <= max_col)
                std::cerr << hheader[i].toStdString() << sep.toStdString() << " ";
        }
        std::cerr << std::endl;
    }
    for (QModelIndex i: indexes) {
        QVariant var = table->model()->data(i);
        if (!var.canConvert<double>())
            continue;
        if (ui->export_header->isChecked()) {
            if (i.column() == min_col) {
                std::cerr << vheader[i.row()].toStdString() << sep.toStdString() << " ";
            }
        }
        if (i.column() < max_col)
            std::cerr << var.value<double>() << sep.toStdString() << " ";
        else
            std::cerr << var.value<double>() << std::endl;
    }

    if (!ui->export_selection->isChecked())
        table->clearSelection();
    if (temp->selection().count() > 0)
        table->setSelectionModel(temp);

}

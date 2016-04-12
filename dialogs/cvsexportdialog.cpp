#include "cvsexportdialog.h"
#include "ui_cvsexportdialog.h"
#include <iostream>
#include <Qt>
#include <QFileDialog>
#include <QTextStream>

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
    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setNameFilter(tr("CVS File (*.cvs)"));
    fd.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fd.exec())
        fileNames = fd.selectedFiles();
    if (fileNames.size() == 0)
        return;
    //QString filename = QFileDialog::getSaveFileName(this, tr("save cvs file"), QString(), tr("CVS files (*.cvs)"));
    QItemSelectionModel* temp = table->selectionModel();
    if (!ui->export_selection->isChecked() || temp->selection().count() == 0) {
        table->selectAll();
    }

    QModelIndexList indexes = table->selectionModel()->selection().indexes();
    int min_col = indexes[0].column();
    int max_col = indexes.back().column();

    QStringList vheader, hheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader);
    }

    QString sep = ui->separator_edit->text();

    QFile outfile(fileNames[0]);
    outfile.open(QIODevice::WriteOnly);

    if(!outfile.isOpen()){
        std::cerr << "- Error, unable to open" << "outputFilename" << "for output";
    }
    QTextStream outStream(&outfile);

    if (ui->export_header->isChecked()) {
        outStream << " " << sep << " ";
        for (int i = 0; i < hheader.size(); i++) {
            if (i >= min_col && i <= max_col) {
                outStream << hheader[i] << sep << " ";
            }
        }
        outStream << "\n";
    }
    for (QModelIndex i: indexes) {
        QVariant var = table->model()->data(i);
        if (!var.canConvert<double>())
            continue;
        if (ui->export_header->isChecked()) {
            if (i.column() == min_col) {
                outStream << vheader[i.row()] << sep << " ";
            }
        }
        if (i.column() < max_col) {
            outStream << var.value<double>() << sep << " ";
        }
        else {
            outStream << var.value<double>() << "\n";
        }
    }

    if (!ui->export_selection->isChecked())
        table->clearSelection();
    outfile.close();
    this->close();
}

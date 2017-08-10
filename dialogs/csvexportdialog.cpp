#include "csvexportdialog.h"
#include "ui_csvexportdialog.h"
#include <iostream>
#include <Qt>
#include <QFileDialog>
#include <QTextStream>
#include <math.h>

#include<QTime>


CSVExportDialog::CSVExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVExportDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);
}

CSVExportDialog::~CSVExportDialog()
{
    delete ui;
}


void CSVExportDialog::set_table(QTableView *table) {
    this->table = table;
}


void CSVExportDialog::accept() {
    export_csv();
}


void CSVExportDialog::get_header(QStringList &vheader, QStringList &hheader) {
    for (int i = 0; i < table->model()->columnCount(); i++)
      hheader.push_back(table->model()->headerData(i, Qt::Horizontal).toString());
    for (int i = 0; i < table->model()->rowCount(); i++)
        vheader.push_back(table->model()->headerData(i, Qt::Vertical).toString());
}


void CSVExportDialog::export_csv() {
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(0);
    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setNameFilter(tr("CSV File (*.csv)"));
    fd.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fd.exec())
        fileNames = fd.selectedFiles();
    if (fileNames.size() == 0)
        return;
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
    ui->progressBar->setValue(5);
    QCoreApplication::processEvents();
    double step = 95. / this->table->model()->rowCount();
    int count = 0;
    //Test stuff to find faster implementation:
    QTime timer;
    timer.start();

    //given Implemetation. first ~500ms second ~400ms for the eod-data in test file

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
        ui->progressBar->setValue(5 + rint(count * step));
        QCoreApplication::processEvents();
        count++;
    }
    std::cerr << "Run time for csv-export was: " << timer.elapsed() << "ms" << std::endl;


    if (!ui->export_selection->isChecked())
        table->clearSelection();
    outfile.close();
    this->close();
}

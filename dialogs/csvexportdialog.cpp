#include "csvexportdialog.h"
#include "ui_csvexportdialog.h"
#include <iostream>
#include <Qt>
#include <QFileDialog>
#include <QTextStream>
#include <math.h>

#include<QTime>
#include "nix.hpp"


CSVExportDialog::CSVExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVExportDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);

    minCol = -1;
    maxCol = -1;
}

CSVExportDialog::~CSVExportDialog()
{
    delete ui;
}


void CSVExportDialog::setArray(nix::DataArray array) {
    this->array = array;
}

void CSVExportDialog::setSelection(int minCol, int maxCol) {
    this->minCol = minCol;
    this->maxCol = maxCol;
}


void CSVExportDialog::accept() {
    export_csv();
}


void CSVExportDialog::get_header(QStringList &vheader, QStringList &hheader) {

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

    QFile outfile(fileNames[0]);
    outfile.open(QIODevice::WriteOnly);

    if(!outfile.isOpen()){
        std::cerr << "- Error, unable to open" << "outputFilename" << "for output";
    }
    QTextStream outStream(&outfile);


    //**********
    if((minCol == -1) | (maxCol == -1)) {
        minCol = 0;
        // maxCol = array. maxCol??
    }
    /*
    QStringList vheader, hheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader);
    }

    QString sep = ui->separator_edit->text().append(" ");


    if (ui->export_header->isChecked()) {
        outStream << " " << sep;
        for (int i = 0; i < hheader.size(); i++) {
            if (i >= minCol && i <= maxCol) {
                outStream << hheader[i] << sep;
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
            if (i.column() == minCol) {
                outStream << vheader[i.row()] << sep;
            }
        }
        if (i.column() < maxCol) {
            outStream << var.value<double>() << sep;
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
        */
    outfile.close();
    this->close();
}

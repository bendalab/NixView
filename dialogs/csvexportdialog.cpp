#include "csvexportdialog.h"
#include "ui_csvexportdialog.h"
#include <iostream>
#include <Qt>
#include <QFileDialog>
#include <QTextStream>
#include <math.h>

#include<QTime>
#include "nix.hpp"
#include "nix/NDArray.hpp"



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


void CSVExportDialog::get_header(QStringList &vheader, QStringList &hheader, QStringList &dheader) {
    nix::NDSize shape = array.dataExtent();
    vheader = QStringList();
    hheader = QStringList();
    dheader = QStringList();

    for(unsigned int i=0; i<shape.size(); i++) {
        if(i == 0) {
            vheader = readLabels(1, array.getDimension(1).dimensionType());
        } else if(i == 1) {
            hheader = readLabels(2, array.getDimension(2).dimensionType());
        } else if(i == 2) {
            dheader = readLabels(3, array.getDimension(3).dimensionType());
        } else {
            break;
        }
    }
}

QStringList CSVExportDialog::readLabels(int dim, nix::DimensionType type) {
    QStringList labels = QStringList();

    if(type == nix::DimensionType::Sample) {
        double si = array.getDimension(dim).asSampledDimension().samplingInterval();
        for(unsigned int i = 0; i<array.dataExtent()[dim-1]; i++) {
            labels.append(QString::number(i*si));
        }
    } else if(type == nix::DimensionType::Set) {
        std::vector<std::string> stdLabels = array.getDimension(dim).asSetDimension().labels();
        for(std::vector<std::string>::iterator it = stdLabels.begin(); it != stdLabels.end(); ++it) {
            labels.append(QString::fromStdString(*it));
        }
    } else if(type == nix::DimensionType::Range) {
        std::vector<double> ticks = array.getDimension(dim).asRangeDimension().ticks();
        for(std::vector<double>::iterator it = ticks.begin(); it != ticks.end(); ++it) {
            labels.append(QString::number(*it));
        }
    }
    return labels;
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

    QStringList vheader, hheader, dheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader, dheader);
    }
    std::cerr << "not the headers." << std::endl;
    QString sep = ui->separator_edit->text().append(" ");



    int xLength, yLength, zLength;
    nix::NDSize shape = array.dataExtent();

    //as extra function ?
    if(shape.size() == 1) {
        xLength = shape[0];
        yLength = 1;
        zLength = 1;
    } else if(shape.size() == 2) {
        xLength = shape[0];
        yLength = shape[1];
        zLength = 1;
    } else if(shape.size() == 3) {
        xLength = shape[0];
        yLength = shape[1];
        zLength = shape[2];
    }

    nix::DataType type = array.dataType();

    for(int z=0; z<zLength; z++) {
        //write dheader+\n if there is one:
        //write hheaders:
        if (ui->export_header->isChecked()) {
            if(zLength > 1) {
                if(z < dheader.size()) {
                    outStream << dheader[z];
                }
                    outStream << "\n";
            }
            outStream << " " << sep;
            for (int i = 0; i < hheader.size(); i++) {
                outStream << hheader[i] << sep;
            }
            outStream << "\n";
        }

        for(int x=0; x<xLength; x++) {
            if(ui->export_header->isChecked()) {
                outStream << vheader[x] << sep;
            }

            nix::NDArray data = nix::NDArray(type, {1,yLength,1});
            // inefficent to get the data line for line ? (better as to not kill the program with large data volumes?)
            if(shape.size() == 1) {
                array.getDataDirect(type, data.data(), {1},            {x});
            } else if(shape.size() == 2) {
                array.getDataDirect(type, data.data(), {1, yLength},   {x, 0});
            } else if(shape.size() > 2) {
                array.getDataDirect(type, data.data(), {1, yLength,1}, {x, 0, z});
            }

            for( nix::ndsize_t y=0; y<yLength; y++) {
                nix::NDSize yIndex(3,1);
                yIndex[1] = y;
                //aufdröseln nach allen typen:
                outStream << data.get<double>(yIndex) << sep;
            }

            outStream << "\n";
        }
    }
    outfile.close();
    this->close();
}



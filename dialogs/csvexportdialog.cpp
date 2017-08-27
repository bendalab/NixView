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
}

CSVExportDialog::~CSVExportDialog()
{
    delete ui;
}


void CSVExportDialog::setArray(nix::DataArray array) {
    this->array = array;
}

void CSVExportDialog::setSelection(nix::NDSize start, nix::NDSize end) {
    this->start = start;
    this->end = end;
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
    nix::NDArray data = nix::NDArray(type, {1});
    if(shape.size() == 1) {
        data = nix::NDArray(type, {1});
    } else if(shape.size() == 2) {
        data = nix::NDArray(type, {1, yLength});
    } else if(shape.size() == 3) {
        data = nix::NDArray(type, {1, yLength,1});
    }

    int count =0;
    double step = 100. / (zLength*xLength);

    for(int z=0; z<zLength; z++) {
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

            // inefficent to get the data line for line ? (better as to not kill the program with large data volumes?)
            if(shape.size() == 1) {
                array.getDataDirect(type, data.data(), {1},            {x});
            } else if(shape.size() == 2) {
                array.getDataDirect(type, data.data(), {1, yLength},   {x, 0});
            } else if(shape.size() == 3) {
                array.getDataDirect(type, data.data(), {1, yLength,1}, {x, 0, z});
            }

            for( nix::ndsize_t y=0; y<yLength; y++) {

                nix::NDSize yIndex;
                if(shape.size() == 1) {
                    yIndex = nix::NDSize(1,y);
                } else if(shape.size() == 2) {
                    yIndex = nix::NDSize(2,0);
                    yIndex[1] = y;
                } else if(shape.size() == 3) {
                    yIndex = nix::NDSize(3,0);
                    yIndex[1] = y;
                }

                //export depending on type:
                exportData(outStream, data, yIndex, sep);
            }

            outStream << "\n";

            count++;
            ui->progressBar->setValue(rint(count * step));
            QCoreApplication::processEvents();

        }
    }
    outfile.close();
    this->close();
}

void CSVExportDialog::exportData(QTextStream &outStream, nix::NDArray &data, nix::NDSize &yIndex, QString &sep) {
    nix::DataType type = data.dtype();

    // how do i get the correct form of int to export depending on machine and compiler ?
    if(type == nix::DataType::Int64) {
        outStream << data.get<long long int>(yIndex) << sep;
    } else if(type == nix::DataType::Int32) {
        outStream << data.get<int>(yIndex) << sep;
    } else if(type == nix::DataType::Int16) {
        outStream << QString::number(data.get<short int>(yIndex)) << sep;
    } else if(type == nix::DataType::Int8) {
        outStream << (int)data.get<char>(yIndex) << sep;
    } else if(type == nix::DataType::Double) {
        outStream << data.get<double>(yIndex) << sep;
    } else if (type == nix::DataType::Float) {
        outStream << data.get<float>(yIndex) << sep;
    }  else if(type == nix::DataType::UInt64) {
        outStream << data.get<unsigned long long int>(yIndex) << sep;
    } else if(type == nix::DataType::UInt32) {
        outStream << data.get<unsigned int>(yIndex) << sep;
    } else if(type == nix::DataType::UInt16) {
        outStream << data.get<unsigned short int>(yIndex) << sep;
    } else if(type == nix::DataType::UInt8) {
        outStream << (unsigned int)data.get<unsigned char>(yIndex) << sep;
    } else if(type == nix::DataType::Bool) {
        outStream << data.get<bool>(yIndex) << sep;
    } else if(type == nix::DataType::Char) {
        outStream << data.get<char>(yIndex) << sep;
    } else if(type == nix::DataType::String) {
        outStream << QString::fromStdString(data.get<std::string>(yIndex)) << sep;
    } else if ((type == nix::DataType::Nothing) | (type == nix::DataType::Opaque))
        //error message ?
        return;
}



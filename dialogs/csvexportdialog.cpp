#include "csvexportdialog.h"
#include "ui_csvexportdialog.h"
#include <iostream>
#include <Qt>
#include <QFileDialog>
#include <QTextStream>

#include<QPushButton>
#include "nix.hpp"
#include "nix/NDArray.hpp"



CSVExportDialog::CSVExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVExportDialog),
    start(1,0), extend(1,0)
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

void CSVExportDialog::setSelection(nix::NDSize start, nix::NDSize extend) {
    this->start = start;
    this->extend = extend;

    if(testStartExtend()) {
        ui->export_selection->setChecked(true);
    }
}

void CSVExportDialog::setSelectionStatus(bool enabled) {
    ui->export_selection->setEnabled(enabled);
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
            if(shape.size() == 1) {
                QString label = QString::fromStdString(array.label().get());
                QString unit = QString::fromStdString(array.unit().get());
                label = label.append(" ").append(unit);
                hheader.append(label);
            }
            vheader = readLabels(1, array.getDimension(1).dimensionType());
        } else if(i == 1) {
            hheader = readLabels(2, array.getDimension(2).dimensionType());
        } else if(i == 2) {
            dheader = readLabels(3, array.getDimension(3).dimensionType());
        } else {
            break;
        }
    }

    if(vheader.isEmpty()) {
        for(unsigned int i=0; i<shape[0]; i++) {
            vheader.append(QString::number(i));
        }
    }
    if(hheader.isEmpty() && shape.size() > 1) {
        for(unsigned int i=0; i<shape[1]; i++) {
            hheader.append(QString::number(i));
        }
    }
    if(dheader.isEmpty() && shape.size() > 2) {
        for(unsigned int i=0; i<shape[2]; i++) {
            dheader.append(QString::number(i));
        }
    }
    return;
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
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
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

    QString sep = ui->separator_edit->text().append(" ");

    if(array.dataExtent().size() == 1) {
        exportCsv1D(outStream, sep);
    } else if(array.dataExtent().size() == 2) {
        exportCsv2D(outStream, sep);
    } else if(array.dataExtent().size() == 3) {
        exportCsv3D(outStream, sep);
    } else if(array.dataExtent().size() > 3) {
        std::cerr << "CSV-export of data with more than 3 dimensions currently not possible. Sorry!" << std::endl;
    }

    outfile.close();
    this->close();
}

void CSVExportDialog::exportCsv1D(QTextStream &outStream, QString &sep) {
    QStringList vheader, hheader, dheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader, dheader);
    }
    nix::NDSize shape = array.dataExtent();
    nix::DataType type = array.dataType();
    nix::NDArray data = nix::NDArray(type, {1});

    if (ui->export_header->isChecked()) {
        outStream << " " << sep;
        for (int i = 0; i < hheader.size(); i++) {
            outStream << hheader[i] << sep;
        }
        outStream << "\n";
    }

    if(! (ui->export_selection->isChecked()) || !testStartExtend()) {
        start[0] = 0;
        extend[0] = shape[0];
    }

    int count =0;
    int draw = 100;
    double step = 100. / extend[0];



    for(unsigned int x = start[0]; x < start[0]+extend[0]; x++) {
        if(ui->export_header->isChecked()) {
            outStream << vheader[x] << sep;
        }

        array.getDataDirect(type, data.data(), {1}, {x});
        nix::NDSize index = nix::NDSize(1,0);
        //export depending on type:
        exportData(outStream, data, index, sep);

        outStream << "\n";

        count++;
        if(count%draw == 0) {
            ui->progressBar->setValue(rint(count * step));
            QCoreApplication::processEvents();
        }
    }
}

void CSVExportDialog::exportCsv2D(QTextStream &outStream, QString &sep) {
    QStringList vheader, hheader, dheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader, dheader);
    }
    nix::NDSize shape = array.dataExtent();
    nix::DataType type = array.dataType();

    if(! ui->export_selection->isChecked() || ! testStartExtend()) {
        start = nix::NDSize(2,0);
        extend   = nix::NDSize(2, shape[0]);
        extend[1] = shape[1];
    }

    nix::NDArray data = nix::NDArray(type, {1, (int) extend[1]});

    int count =0;
    int draw = 100;
    double step = 100. / extend[0];

    if(ui->export_header->isChecked()) {
        outStream << " " << sep;
        for (unsigned int i = start[1]; i < start[1]+extend[1]; i++) {
              outStream << hheader[i] << sep;
        }
        outStream << "\n";
    }

    for( int x = start[0]; x < (int)(start[0]+extend[0]); x++) {
        if(ui->export_header->isChecked()) {
            outStream << vheader[x] << sep;
        }
        array.getDataDirect(type, data.data(), {1, (int) extend[1]},   {x, (int)start[1]});

        for(unsigned int y = 0; y<extend[1]; y++) {
            nix::NDSize yIndex;
            yIndex = nix::NDSize(2,0);
            yIndex[1] = y;
            //export depending on type:
            exportData(outStream, data, yIndex, sep);
        }
        outStream << "\n";

        count++;
        if(count%draw == 0) {
            ui->progressBar->setValue(rint(count * step));
            QCoreApplication::processEvents();
        }
    }
}

void CSVExportDialog::exportCsv3D(QTextStream &outStream, QString &sep) {
    QStringList vheader, hheader, dheader;
    if (ui->export_header->isChecked()) {
        get_header(vheader, hheader, dheader);
    }
    nix::NDSize shape = array.dataExtent();
    nix::DataType type = array.dataType();

    if( ! ui->export_selection->isChecked() || ! testStartExtend()) {
        start = nix::NDSize(3,0);
        extend = nix::NDSize(3,shape[0]);
        extend[1] = shape[1];
        extend[2] = shape[2];
    }

    nix::NDArray data = nix::NDArray(type, {1, (int) extend[1], 1});

    int count =0;
    int draw = 100;
    double step = 100. / extend[2]*extend[0]; // number of lines with data.

    for(int z = start[2] ; z < (int)(start[2]+extend[2]); z++) {
        if (ui->export_header->isChecked()) {

            if(z < dheader.size()) {
                outStream << dheader[z];
                outStream << "\n";
            }

            outStream << " " << sep;
            for (unsigned int i = start[1]; i < start[1]+extend[1]; i++) {
                outStream << hheader[i] << sep;
            }
            outStream << "\n";
        }

        for(int x=start[0]; x < (int) (start[0]+extend[0]); x++) {
            if(ui->export_header->isChecked()) {
                outStream << vheader[x] << sep;
            }

            array.getDataDirect(type, data.data(), { 1, (int) extend[1], 1}, {x, (int) start[1], z});
            for(unsigned int y=0; y<extend[1]; y++) {

                nix::NDSize yIndex;
                yIndex = nix::NDSize(3,0);
                yIndex[1] = y;

                //export depending on type:
                exportData(outStream, data, yIndex, sep);
            }

            outStream << "\n";

            count++;
            if(count%draw == 0) {
                ui->progressBar->setValue(rint(count * step));
                QCoreApplication::processEvents();
            }
        }

    }
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
    } else if(type == nix::DataType::UInt64) {
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

bool CSVExportDialog::testStartExtend() {
    nix::NDSize shape;
    array.dataExtent(shape);
    if( (shape.size() != start.size()) || (shape.size() != extend.size()) ) {
        return false;
    } else if( start.size() != extend.size()) {
        return false;
    }
    for(unsigned int i=0; i<extend.size(); i++) {
        if(extend[i] == 0) {
            return false;
        }
        if(start[i]+ extend[i] > shape[i]) {
            return false;
        }
    }
    return true;
}







#ifndef CSVEXPORTDIALOG_H
#define CSVEXPORTDIALOG_H

#include <QDialog>
#include <QTextStream>
#include "nix.hpp"

namespace Ui {
class CSVExportDialog;
}

class CSVExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSVExportDialog(QWidget *parent = 0);
    ~CSVExportDialog();

    void setArray(nix::DataArray array);
    void setSelection(nix::NDSize start, nix::NDSize extend);
    void setSelectionStatus(bool enabled);

private:
    Ui::CSVExportDialog *ui;
    nix::DataArray array;
    nix::NDSize start,extend;
    unsigned int partLength;

    void accept();
    void export_csv();

    void exportCsv1D(QTextStream &outStream, QString &sep);
    void exportCsv2D(QTextStream &outStream, QString &sep);
    void exportCsv3D(QTextStream &outStream, QString &sep);

    //void get_header(QStringList &vheader, QStringList &hheader, QStringList &dheader);
    //QStringList readLabels(int dim, nix::DimensionType type);
    QStringList getHeaders(unsigned int dim, unsigned int current, unsigned int length);
    void export1DHeader(QStringList& vheader, QTextStream&  outStream, QString &sep, unsigned int x_ind);
    void export2DHeader(QStringList& hheader, QTextStream&  outStream, QString &sep);
    void exportData(QTextStream &outStream, nix::NDArray &data, nix::NDSize &yIndex, QString &sep);
    bool testStartExtend();
};

#endif // CSVEXPORTDIALOG_H

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
    void setSelection(nix::NDSize start, nix::NDSize end);

private:
    Ui::CSVExportDialog *ui;
    nix::DataArray array;
    nix::NDSize start,end;

    void accept();
    void export_csv();
    void get_header(QStringList &vheader, QStringList &hheader, QStringList &dheader);
    QStringList readLabels(int dim, nix::DimensionType type);
    void exportData(QTextStream &outStream, nix::NDArray &data, nix::NDSize &yIndex, QString &sep);
};

#endif // CSVEXPORTDIALOG_H

#ifndef CSVEXPORTDIALOG_H
#define CSVEXPORTDIALOG_H

#include <QDialog>
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
    void setSelection(int minCol, int maxCol);

private:
    Ui::CSVExportDialog *ui;
    nix::DataArray array;
    int minCol, maxCol;

    void accept();
    void export_csv();
    void get_header(QStringList &vheader, QStringList &hheader);
};

#endif // CSVEXPORTDIALOG_H

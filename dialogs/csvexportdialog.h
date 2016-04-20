#ifndef CSVEXPORTDIALOG_H
#define CSVEXPORTDIALOG_H

#include <QDialog>
#include <QTableView>

namespace Ui {
class CSVExportDialog;
}

class CSVExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSVExportDialog(QWidget *parent = 0);
    ~CSVExportDialog();

    void set_table(QTableView *table);

private:
    Ui::CSVExportDialog *ui;
    QTableView *table;

    void accept();
    void export_csv();
    void get_header(QStringList &vheader, QStringList &hheader);
};

#endif // CSVEXPORTDIALOG_H

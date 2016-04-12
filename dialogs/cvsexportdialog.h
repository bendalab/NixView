#ifndef CVSEXPORTDIALOG_H
#define CVSEXPORTDIALOG_H

#include <QDialog>
#include <QTableView>

namespace Ui {
class CVSExportDialog;
}

class CVSExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CVSExportDialog(QWidget *parent = 0);
    ~CVSExportDialog();

    void set_table(QTableView *table);

private:
    Ui::CVSExportDialog *ui;
    QTableView *table;

    void accept();
    void export_cvs();
};

#endif // CVSEXPORTDIALOG_H

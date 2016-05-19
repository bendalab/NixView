#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

public slots:
    void file_opened(QString filename);

signals:
    void column_visibility_changed(QString who, QString column, bool state);
    void recent_file_changed(QStringList);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H

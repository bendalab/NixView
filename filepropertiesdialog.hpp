#ifndef FILEPROPERTIESDIALOG_HPP
#define FILEPROPERTIESDIALOG_HPP

#include <QDialog>

namespace Ui {
class FilePropertiesDialog;
}

class FilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertiesDialog(QWidget *parent = 0);
    ~FilePropertiesDialog();

private:
    Ui::FilePropertiesDialog *ui;
};

#endif // FILEPROPERTIESDIALOG_HPP

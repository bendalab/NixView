#ifndef FILEPROPERTIESDIALOG_HPP
#define FILEPROPERTIESDIALOG_HPP

#include <QDialog>
#include <nix.hpp>
#include <boost/filesystem.hpp>

namespace Ui {
class FilePropertiesDialog;
}

class FilePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilePropertiesDialog(QWidget *parent = 0);
    ~FilePropertiesDialog();

    void set_file(const nix::File &file, const boost::filesystem::path &file_path);

private:
    Ui::FilePropertiesDialog *ui;
};

#endif // FILEPROPERTIESDIALOG_HPP

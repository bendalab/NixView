#include "filepropertiesdialog.hpp"
#include "ui_filepropertiesdialog.h"

FilePropertiesDialog::FilePropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilePropertiesDialog)
{
    ui->setupUi(this);
}

FilePropertiesDialog::~FilePropertiesDialog()
{
    delete ui;
}


void FilePropertiesDialog::set_file(const nix::File &file, const boost::filesystem::path &file_path) {

}

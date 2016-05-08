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


int count_sections(nix::Section s) {
   int count = s.sectionCount();
   for (nix::Section subsec : s.sections()) {
       count += count_sections(subsec);
   }
   return count;
}


void FilePropertiesDialog::set_file(const nix::File &file, const boost::filesystem::path &file_path) {
    ui->format->setText(QString::fromStdString(file.format()));
    std::string version;
    for (int i : file.version())
        version = version + nix::util::numToStr(i) + " ";
    ui->version->setText(QString::fromStdString(version));
    ui->created->setText(QString::fromStdString(nix::util::timeToStr(file.createdAt())));
    ui->updated->setText(QString::fromStdString(nix::util::timeToStr(file.updatedAt())));
    ui->block_count->setText(QVariant(file.blockCount()).toString());
    int da_count = 0;
    int tag_count = 0;
    int section_count = 0;
    for (nix::Block b : file.blocks()) {
        da_count += b.dataArrayCount();
        tag_count += b.tagCount();
        tag_count += b.multiTagCount();
    }
    section_count += file.sectionCount();
    for (nix::Section s : file.sections()) {
        section_count += count_sections(s);
    }
    ui->array_count->setText(QVariant(da_count).toString());
    ui->tag_count->setText(QVariant(tag_count).toString());
    ui->section_count->setText(QVariant(section_count).toString());
    ui->file_path->setText(QString(file_path.c_str()));
    ui->file_size->setText(QString::fromStdString(nix::util::numToStr(boost::filesystem::file_size(file_path) / 1000000.) + " MB"));

}

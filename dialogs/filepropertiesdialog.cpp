#include "filepropertiesdialog.hpp"
#include "ui_filepropertiesdialog.h"
#include <bitset>

namespace bfs = boost::filesystem;

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


void FilePropertiesDialog::set_file(const nix::File &file, const bfs::path &file_path) {
    ui->format->setText(QString::fromStdString(file.format()));
    std::string version;
    for (int i : file.version())
        version = version + nix::util::numToStr(i) + ".";
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

    bfs::file_status status = boost::filesystem::status(file_path);
    ui->owner_read->setChecked((bfs::perms::owner_read & status.permissions()) == bfs::perms::owner_read);
    ui->owner_write->setChecked((bfs::perms::owner_write & status.permissions()) == bfs::perms::owner_write);
    ui->owner_exec->setChecked((bfs::perms::owner_exe & status.permissions()) == bfs::perms::owner_exe);

    ui->group_read->setChecked((bfs::perms::group_read & status.permissions()) == bfs::perms::group_read);
    ui->group_write->setChecked((bfs::perms::group_write & status.permissions()) == bfs::perms::group_write);
    ui->group_exec->setChecked((bfs::perms::group_exe & status.permissions()) == bfs::perms::group_exe);

    ui->all_read->setChecked((bfs::perms::others_read & status.permissions()) == bfs::perms::others_read);
    ui->all_write->setChecked((bfs::perms::others_write & status.permissions()) == bfs::perms::others_write);
    ui->all_exec->setChecked((bfs::perms::others_exe & status.permissions()) == bfs::perms::others_exe);
}

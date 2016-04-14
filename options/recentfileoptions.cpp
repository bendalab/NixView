#include "recentfileoptions.hpp"
#include "ui_recentfileoptions.h"
#include "common/Common.hpp"
#include <iostream>
#include <nix.hpp>
#include <QListWidgetItem>

RecentFileOptions::RecentFileOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentFileOptions), recent_files()
{
    ui->setupUi(this);
    settings = new QSettings();
    QObject::connect(ui->file_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this, SLOT(item_selected(QListWidgetItem*, QListWidgetItem*)));
    QObject::connect(ui->down_button, SIGNAL(clicked()), this, SLOT(move_item_down()));
    QObject::connect(ui->up_button, SIGNAL(clicked()), this, SLOT(move_item_up()));
    QObject::connect(ui->delete_button, SIGNAL(clicked()), this, SLOT(delete_item()));
    load_settings();
}


RecentFileOptions::~RecentFileOptions()
{
    delete ui;
}


void RecentFileOptions::load_settings() {
    settings->beginGroup(RECENT_FILES_GROUP);
    ui->file_list->clear();
    recent_files.clear();
    QStringList keys = settings->childKeys();
    for (QString s : keys) {
        recent_files.push_back(settings->value(s).toString());
    }
    fill_list();
    emit recent_files_update(recent_files);
    settings->endGroup();
}


void RecentFileOptions::fill_list() {
    ui->file_list->clear();
    for (QString s : recent_files) {
        ui->file_list->addItem(s);
    }
}


void RecentFileOptions::set_file(QString filename) {
    settings->beginGroup(RECENT_FILES_GROUP);
    recent_files.clear();
    QStringList keys = settings->childKeys();
    for (QString k : keys) {
        recent_files.push_back(settings->value(k).toString());
    }
    if (recent_files.size() > RECENT_FILES_MAX_COUNT) {
        recent_files.pop_back();
    }
    recent_files.insert(0, filename);
    for (int i = recent_files.size() - 1; i > 0; i--) {
        if (recent_files[i].compare(filename) == 0){
            recent_files.removeAt(i);
        }
    }
    settings->remove("");
    for (int i = 0; i < recent_files.size(); i ++) {
        QString key = QString::fromStdString(nix::util::numToStr(i));
        settings->setValue(key, filename);
    }
    settings->endGroup();
    settings->sync();
    fill_list();
    emit recent_files_update(recent_files);
}


void RecentFileOptions::item_selected(QListWidgetItem *current, QListWidgetItem *past) {
    if (current != nullptr) {
        ui->delete_button->setEnabled(true);
        ui->up_button->setEnabled(ui->file_list->currentRow() > 0);
        ui->down_button->setEnabled(ui->file_list->currentRow() < (ui->file_list->count() - 1));
    } else {
        ui->delete_button->setEnabled(false);
        ui->up_button->setEnabled(false);
        ui->down_button->setEnabled(false);
    }
}


void RecentFileOptions::move_item_up() {
    int index = ui->file_list->currentRow();
    if (index > 0) {
        QString s = recent_files[index];
        recent_files.removeAt(index);
        recent_files.insert(index -1, s);
    }
    fill_list();
    emit recent_files_update(recent_files);
}


void RecentFileOptions::move_item_down() {
    int index = ui->file_list->currentRow();
    if (index < (recent_files.size() -1)) {
        QString s = recent_files[index];
        recent_files.removeAt(index);
        recent_files.insert(index + 1, s);
    }
    fill_list();
    emit recent_files_update(recent_files);
}


void RecentFileOptions::delete_item() {
    int index = ui->file_list->currentRow();
    recent_files.removeAt(index);
    fill_list();
    emit recent_files_update(recent_files);
}

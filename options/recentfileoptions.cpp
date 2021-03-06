#include "recentfileoptions.hpp"
#include "ui_recentfileoptions.h"
#include "common/Common.hpp"
#include <iostream>
#include <nix.hpp>
#include <QListWidgetItem>
#include "utils/utils.hpp"


RecentFileOptions::RecentFileOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentFileOptions), recent_files()
{
    ui->setupUi(this);
    QObject::connect(ui->file_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                     this, SLOT(item_selected(QListWidgetItem*, QListWidgetItem*)));
    QObject::connect(ui->down_button, SIGNAL(clicked()), this, SLOT(move_item_down()));
    QObject::connect(ui->up_button, SIGNAL(clicked()), this, SLOT(move_item_up()));
    QObject::connect(ui->delete_button, SIGNAL(clicked()), this, SLOT(delete_item()));
    QObject::connect(ui->spinBox, SIGNAL(editingFinished()), this, SLOT(file_count_update()));
    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(file_count_update()));

    load_settings();
}


RecentFileOptions::~RecentFileOptions()
{
    delete ui;
}


void RecentFileOptions::load_settings() {
    QSettings *settings = new QSettings();
    settings->beginGroup(RECENT_FILES_GROUP);
    int count = settings->value(RECENT_FILES_COUNT, 5).toInt();
    ui->spinBox->setValue(count);
    settings->beginGroup(RECENT_FILES_LIST);
    ui->file_list->clear();
    recent_files.clear();
    QStringList keys = settings->childKeys();
    for (QString s : keys) {
        recent_files.push_back(settings->value(s).toString());
    }
    fill_list();
    emit recent_files_update(recent_files);
    settings->endGroup();
    settings->endGroup();
    delete settings;
}


void RecentFileOptions::fill_list() {
    nixview::util::remove_duplicates(recent_files);
    ui->file_list->clear();
    for (QString s : recent_files) {
        ui->file_list->addItem(s);
    }
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


void RecentFileOptions::file_count_update() {
    while(ui->spinBox->value() < recent_files.size()) {
        recent_files.removeLast();
    }
    fill_list();
    QSettings *settings = new QSettings();
    settings->beginGroup(RECENT_FILES_GROUP);
    settings->setValue(RECENT_FILES_COUNT, ui->spinBox->value());
    settings->endGroup();
    settings->sync();
    delete settings;
    emit recent_files_update(recent_files);
}

#include "recentfileoptions.hpp"
#include "ui_recentfileoptions.h"
#include <iostream>
#include <nix.hpp>

RecentFileOptions::RecentFileOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentFileOptions)
{
    ui->setupUi(this);
    settings = new QSettings();
    load_settings();
}


RecentFileOptions::~RecentFileOptions()
{
    delete ui;
}


void RecentFileOptions::load_settings() {
    settings->beginGroup("Recent_Files");

    QStringList recent_files;
    QStringList keys = settings->childKeys();
    for (QString s : keys) {
        recent_files.push_back(settings->value(s).toString());
    }
    emit recent_files_update(recent_files);
    settings->endGroup();
}


void RecentFileOptions::set_file(QString filename) {
    settings->beginGroup("Recent_Files");
    QStringList recent_files;
    QStringList keys = settings->childKeys();
    for (QString k : keys) {
        recent_files.push_back(settings->value(k).toString());
    }
    if (recent_files.size() > 5) {
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
    emit recent_files_update(recent_files);
}

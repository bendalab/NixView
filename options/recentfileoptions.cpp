#include "recentfileoptions.hpp"
#include "ui_recentfileoptions.h"

RecentFileOptions::RecentFileOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecentFileOptions)
{
    ui->setupUi(this);
}

RecentFileOptions::~RecentFileOptions()
{
    delete ui;
}

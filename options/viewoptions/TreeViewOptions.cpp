#include "TreeViewOptions.hpp"
#include "ui_TreeViewOptions.h"
#include "common/Common.hpp"

TreeViewOptions::TreeViewOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeViewOptions)
{
    ui->setupUi(this);

    ui->checkBox_1->setText(QString(MODEL_HEADER_NAME));
    ui->checkBox_2->setText(QString(MODEL_HEADER_NIXTYPE));
    ui->checkBox_3->setText(QString(MODEL_HEADER_STORAGETYPE));
    ui->checkBox_4->setText(QString(MODEL_HEADER_DATATYPE));
    ui->checkBox_5->setText(QString(MODEL_HEADER_SHAPE));
    ui->checkBox_6->setText(QString(MODEL_HEADER_ID));
    ui->checkBox_7->setText(QString(MODEL_HEADER_CREATEDAT));
    ui->checkBox_8->setText(QString(MODEL_HEADER_UPDATEDAT));
    ui->checkBox_9->setText(QString(MODEL_HEADER_VALUE));
    ui->checkBox_10->setText(QString(MODEL_HEADER_UNERTAINTY));
}

TreeViewOptions::~TreeViewOptions()
{
    delete ui;
}

#include "TreeViewOptions.hpp"
#include "ui_TreeViewOptions.h"
#include "common/Common.hpp"
#include "views/RawTreeView.hpp"

TreeViewOptions::TreeViewOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeViewOptions)
{
    ui->setupUi(this);
    settings = new QSettings;

    ui->checkBox_name->setText(QString(MODEL_HEADER_NAME));
    ui->checkBox_nixtype->setText(QString(MODEL_HEADER_NIXTYPE));
    ui->checkBox_storagetype->setText(QString(MODEL_HEADER_STORAGETYPE));
    ui->checkBox_datatype->setText(QString(MODEL_HEADER_DATATYPE));
    ui->checkBox_shape->setText(QString(MODEL_HEADER_SHAPE));
    ui->checkBox_id->setText(QString(MODEL_HEADER_ID));
    ui->checkBox_createdat->setText(QString(MODEL_HEADER_CREATEDAT));
    ui->checkBox_updatedat->setText(QString(MODEL_HEADER_UPDATEDAT));
    ui->checkBox_value->setText(QString(MODEL_HEADER_VALUE));
    ui->checkBox_uncertainty->setText(QString(MODEL_HEADER_UNERTAINTY));

    load_settings();

    connect_widgets();
}

void TreeViewOptions::load_settings()
{
    settings->beginGroup(S_RAWTREEVIEW);
    settings->beginGroup(S_COLUMNSHIDDEN);

    QStringList default_hidden_columns = RawTreeView::get_default_hidden_columns();

    ui->checkBox_name->setChecked(!(settings->value(MODEL_HEADER_NAME, default_hidden_columns.contains(MODEL_HEADER_NAME) ? true : false).toBool()));
    ui->checkBox_nixtype->setChecked(!(settings->value(MODEL_HEADER_NIXTYPE, default_hidden_columns.contains(MODEL_HEADER_NIXTYPE) ? true : false).toBool()));
    ui->checkBox_storagetype->setChecked(!(settings->value(MODEL_HEADER_STORAGETYPE, default_hidden_columns.contains(MODEL_HEADER_STORAGETYPE) ? true : false).toBool()));
    ui->checkBox_datatype->setChecked(!(settings->value(MODEL_HEADER_DATATYPE, default_hidden_columns.contains(MODEL_HEADER_DATATYPE) ? true : false).toBool()));
    ui->checkBox_shape->setChecked(!(settings->value(MODEL_HEADER_SHAPE, default_hidden_columns.contains(MODEL_HEADER_SHAPE) ? true : false).toBool()));
    ui->checkBox_id->setChecked(!(settings->value(MODEL_HEADER_ID, default_hidden_columns.contains(MODEL_HEADER_ID) ? true : false).toBool()));
    ui->checkBox_createdat->setChecked(!(settings->value(MODEL_HEADER_CREATEDAT, default_hidden_columns.contains(MODEL_HEADER_CREATEDAT) ? true : false).toBool()));
    ui->checkBox_updatedat->setChecked(!(settings->value(MODEL_HEADER_UPDATEDAT, default_hidden_columns.contains(MODEL_HEADER_UPDATEDAT) ? true : false).toBool()));
    ui->checkBox_value->setChecked(!(settings->value(MODEL_HEADER_VALUE, default_hidden_columns.contains(MODEL_HEADER_VALUE) ? true : false).toBool()));
    ui->checkBox_uncertainty->setChecked(!(settings->value(MODEL_HEADER_UNERTAINTY, default_hidden_columns.contains(MODEL_HEADER_UNERTAINTY) ? true : false).toBool()));

    settings->endGroup();
    settings->endGroup();
}

void TreeViewOptions::connect_widgets()
{
    QObject::connect(ui->checkBox_name, SIGNAL(clicked(bool)), this, SLOT(set_name_column_hidden(bool)));
}

void TreeViewOptions::set_name_column_hidden(bool b)
{
    settings->beginGroup(S_RAWTREEVIEW);
    settings->beginGroup(S_COLUMNSHIDDEN);
    settings->setValue(MODEL_HEADER_NAME, !b);
    settings->endGroup();
    settings->endGroup();
}

TreeViewOptions::~TreeViewOptions()
{
    delete ui;
}

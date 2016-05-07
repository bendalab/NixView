#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    QObject::connect(ui->widget, SIGNAL(column_state_changed(QString,bool)),
                     this, SIGNAL(column_visibility_changed(QString,bool)));
    QObject::connect(ui->widget, SIGNAL(recent_file_update(QStringList)),
                     this, SIGNAL(recent_file_changed(QStringList)));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::file_opened(QString filename)
{
    QObject::connect(this, SLOT(file_opened(QString)), ui->widget, SLOT(file_opened(QString)));
}

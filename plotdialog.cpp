#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "common/Common.hpp"
#include <QToolBar>
#include <QMenu>
#include <QInputDialog>


PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog) {
    ui->setupUi(this);
}

void PlotDialog::set_entity(QVariant var) {
    ui->plot->setEntity(var);
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

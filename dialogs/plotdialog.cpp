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

    QPushButton *btnClose = new QPushButton(tr("&Close"));
    ui->buttonBox->addButton(btnClose, QDialogButtonBox::AcceptRole);

    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

void PlotDialog::set_entity(QVariant var) {
    if (var.canConvert<nix::Tag>() || var.canConvert<nix::MultiTag>()) {
        ui->tag_view->setEntity(var);
        ui->stackedWidget->setCurrentIndex(1);
        resize(800, 650);
    } else {
        ui->plot->setEntity(var);
        ui->stackedWidget->setCurrentIndex(0);
        resize(640, 240);
    }
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

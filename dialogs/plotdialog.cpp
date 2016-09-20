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

void PlotDialog::set_entity(QModelIndex qml) {
    ui->plot->setEntity(qml);
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

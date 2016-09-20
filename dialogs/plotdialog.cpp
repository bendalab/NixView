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
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());

    if ((item->nixType() == NixType::NIX_TAG) || (item->nixType() == NixType::NIX_MTAG)){
        ui->tag_view->setEntity(qml);
        ui->stackedWidget->setCurrentIndex(1);
        resize(800, 650);
    } else {
        ui->plot->setEntity(qml);
        ui->stackedWidget->setCurrentIndex(0);
        resize(640, 240);
    }
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

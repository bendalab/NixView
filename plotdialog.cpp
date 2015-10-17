#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "common/Common.hpp"


PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);
}


void PlotDialog::set_entity(QVariant var) {
    std::cerr << "set entity" << std::endl;
    this->item = var;
    if (can_draw()) {
        draw();
    }
}


void PlotDialog::draw() {
    std::cerr << "draw" << std::endl;

}


bool PlotDialog::can_draw() {
    std::cerr << "can draw" << std::endl;
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() | item.canConvert<nix::Tag>();
}


PlotDialog::~PlotDialog()
{
    delete ui;
}

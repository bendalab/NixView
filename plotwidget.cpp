#include "plotwidget.h"
#include "ui_plotwidget.h"
#include "lineplotter.h"
#include "common/Common.hpp"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    std::cerr << "PlotWidget" << std::endl;
}

PlotWidget::~PlotWidget()
{
    delete ui;
}


bool PlotWidget::can_draw() const {
    return this->item.canConvert<nix::DataArray>() | this->item.canConvert<nix::MultiTag>() | this->item.canConvert<nix::Tag>();
}
void PlotWidget::setEntity(QVariant var) {
    this->item = var;
    select_plotter();
    if (can_draw()) {
        draw();
    }
}


bool PlotWidget::check_plottable_dtype(nix::DataType dtype) const {
    bool plottable = true;
    plottable = plottable && dtype != nix::DataType::Bool;
    plottable = plottable && dtype != nix::DataType::String;
    plottable = plottable && dtype != nix::DataType::Char;
    plottable = plottable && dtype != nix::DataType::Opaque;
    plottable = plottable && dtype != nix::DataType::Nothing;
    return plottable;
}

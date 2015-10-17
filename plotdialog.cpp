#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "common/Common.hpp"

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);
}


QCustomPlot* PlotDialog::get_plot() {
    return ui->plot;
}

void PlotDialog::set_entity(QVariant var) {
    this->item = var;
    if (can_draw()) {
        draw();
    }
}


void PlotDialog::draw() {
    if (item.canConvert<nix::DataArray>()) {
        nix::DataArray array = item.value<nix::DataArray>();
        ui->plot->clearGraphs();
        ui->plot->addGraph();
        ui->plot->setInteraction(QCP::iRangeDrag, true);
        ui->plot->setInteraction(QCP::iRangeZoom, true);
        ui->plot->axisRect()->setRangeZoomFactor(2.0, 1.0);
        size_t dim_count = array.dimensionCount();
        switch (dim_count) {
        case 1:
            draw_1d(array);
            break;
        case 2:
            draw_2d(array);
            break;
        default:
            break;
        }
    } else {
        std::cerr << "Sorry, plotting of Tags and MultiTags is not yet supported." << std::endl;
    }
}


void PlotDialog::draw_1d(const nix::DataArray &array) {
    nix::Dimension d = array.getDimension(1);
    QVector<double> x_axis, y_axis;
    double x_min = 0.0, x_max=1.0, y_min=-1.0, y_max=1.0;
    std::string x_label, y_label;
    if (d.dimensionType() == nix::DimensionType::Sample) {
        nix::SampledDimension dim = d.asSampledDimension();
        std::vector<double> ax = dim.axis(array.dataExtent()[0]);
        x_axis = QVector<double>::fromStdVector(ax);
        x_min = ax[0];
        x_max = ax.back();
        if (dim.label())
            x_label = *dim.label();
        if (dim.unit())
            x_label = x_label + " [" + *dim.unit() + "]";

        std::vector<double> data(ax.size());
        array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
        y_axis = QVector<double>::fromStdVector(data);
        y_min = *std::min_element(std::begin(data), std::end(data));
        y_max = *std::max_element(std::begin(data), std::end(data));
        if (array.label())
            y_label = *array.label();
        if (array.unit())
            y_label = y_label + " [" + *array.unit() + "]";
    } else if (d.dimensionType() == nix::DimensionType::Range) {
        std::cerr << "1D sampled" << std::endl;

    } else {

    }
    ui->plot->graph(0)->setData(x_axis, y_axis);
    ui->plot->xAxis->setRange(x_min, x_max);
    ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    ui->plot->xAxis->setLabel(QString(x_label.c_str()));
    ui->plot->yAxis->setLabel(QString(y_label.c_str()));
    ui->plot->replot();
}


void PlotDialog::draw_2d(const nix::DataArray &array) {

}


bool PlotDialog::can_draw() {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() | item.canConvert<nix::Tag>();
}


PlotDialog::~PlotDialog()
{
    delete ui;
}

#include "imageplotter.h"
#include "ui_imageplotter.h"
#include <QMenu>


ImagePlotter::ImagePlotter(QWidget *parent) :
        QWidget(parent), ui(new Ui::ImagePlotter), cmap() {
    ui->setupUi(this);

}

ImagePlotter::~ImagePlotter() {
    delete ui;
}

QCustomPlot *ImagePlotter::get_plot() {
    return ui->plot;
}

struct DimInfo {
    bool valid;
    QString unit;
    QString label;

    QVector<double> ticks;
    QCPRange range;
};

static DimInfo get_dim_info(const nix::DataArray &array, nix::ndsize_t dim_index) {
    nix::Dimension d = array.getDimension(dim_index);
    DimInfo info;
    info.valid = false;

    nix::NDSize shape = array.dataExtent();

    if (d.dimensionType() == nix::DimensionType::Sample) {
        nix::SampledDimension dim = d.asSampledDimension();
        info.valid = true;
        info.label = dim.label().get_value_or("").c_str();
        info.unit = dim.unit().get_value_or("").c_str();
        info.ticks = QVector<double>::fromStdVector(dim.axis(shape[dim_index-1]));

    } else if (d.dimensionType() == nix::DimensionType::Range) {
        nix::RangeDimension dim = d.asRangeDimension();
        info.valid = true;
        info.label = dim.label().get_value_or("").c_str();
        info.unit = dim.unit().get_value_or("").c_str();

        info.ticks = QVector<double>::fromStdVector(dim.ticks());
    }

    const int size = info.ticks.size();
    if (size > 0) {
        info.range.lower = info.ticks[0];
        info.range.upper = info.ticks[size - 1];
    }

    return info;
}

void ImagePlotter::draw(const nix::DataArray &array) {
    if (array.dimensionCount() != 2) {
        std::cerr << "ImagePlotter::draw can only draw 2D!" << std::endl;
        return;
    }

    DimInfo yi = get_dim_info(array, 1); // rows == y
    DimInfo xi = get_dim_info(array, 2); // cols == x

    if (!(yi.valid && xi.valid)) {
        std::cerr << "ImagePlotter::invalid dimensions found in array!" << std::endl;
        return;
    }

    QCustomPlot *plot = get_plot();

    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    plot->axisRect()->setupFullAxesBox(true);
    plot->xAxis->setLabel(xi.label + " " + xi.unit);
    plot->yAxis->setLabel(yi.label + " " + yi.unit);

    QCPColorMap *colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);

    int nx = xi.ticks.size();
    int ny = yi.ticks.size();

    colorMap->data()->setSize(nx, ny);
    colorMap->data()->setRange(xi.range, yi.range);
    colorMap->setInterpolate(false);

    for (int xidx = 0; xidx < nx; xidx++) {
        std::vector<double> data(static_cast<size_t>(nx));
        //lets fetch a whole row (should be fast if data is
        //  row-major, like hdf5)
        //ny,   the number of rows in data-array
        //xidx, the current column the fetch
        // -> fetch all rows at column [xidx]
        //                   count    offset
        array.getData(data, {ny, 1}, {0, xidx});
        for (int yidx = 0; yidx < ny; yidx++) {
            // data[ydix], the data point of the row[yidx], col[xidx]
            colorMap->data()->setCell(xidx, yidx, data[yidx]);
        }
    }

    QCPColorScale *colorScale = new QCPColorScale(plot);
    plot->plotLayout()->addElement(0, 1, colorScale);
    colorScale->setType(QCPAxis::atRight);
    colorMap->setColorScale(colorScale);
    if (array.label()) {
        colorScale->axis()->setLabel(array.label().get().c_str());
    }
    colorMap->setGradient(QCPColorGradient::gpSpectrum);
    colorMap->rescaleDataRange();

    QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
    plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    plot->rescaleAxes();
}


void ImagePlotter::add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name,
                              bool y_scale) {

}

void ImagePlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {

}

void ImagePlotter::set_label(const std::string &label) {

}

PlotterType ImagePlotter::plotter_type() const {
    return PlotterType::Image;
}


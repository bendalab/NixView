#include "plotter.h"

Plotter::Plotter(QWidget *parent): QWidget(parent) {


}

void Plotter::add_events(const QVector<double> x_data, const QVector<double> y_data, conts QString &name, bool y_scale) {
    ui->plot->addGraph();
    ui->plot->graph()->addData(x_data, y_data);
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->plot->graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
    ui->plot->graph()->setName(name);
    ui->plot->graph()->setPen(QPen(Qt::red));
    if (y_scale) {
        double y_min = *std::min_element(std::begin(y_data), std::end(y_data));
        double y_max = *std::max_element(std::begin(y_data), std::end(y_data));
        if (y_min == y_max)
            y_min = 0.0;

        ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    }
    ui->plot->xAxis->setRange(x_data[0], x_data.last());
}

void Plotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {
    if (positions.size() != extents.size()) {
        std::cerr << "Cannot draw segments, number of positions and extents does not match!" << std::endl;
    }
    for (int i = 0; i<positions.size(); i++) {
        QCPItemRect *rect = new QCPItemRect(ui->plot);
        double x_min, x_max, y_min, y_max;
        y_max = ui->plot->yAxis->range().upper;
        y_min = ui->plot->yAxis->range().lower;
        x_min = positions[i];
        x_max = x_min + extents[i];
        rect->position("topLeft")->setCoords(x_min, y_max);
        rect->position("bottomRight")->setCoords(x_max, y_min);
        rect->setPen(QPen(Qt::red));
        rect->setBrush(QBrush(QColor(255, 10, 10, 50)));
        ui->plot->addItem(rect);
    }
}


void Plotter::data_array_ax_labels(const nix::DataArray &array, QString &ylabel, QVector<QString> &labels) {
    if (array.label())
        ylabel = QString::fromStdString(*array.label());
    if (array.unit())
        ylabel = ylabel + " [" + QString::fromStdString(*array.unit()) + "]";
    for (nix::ndsize_t i = 0; i < array.dimensionCount(); i++) {
        nix::Dimension d = array.getDimension(i + 1);
        std::string dim_label;
        if (d.dimensionType() == nix::DimensionType::Sample) {
            nix::SampledDimension dim = d.asSampledDimension();
            if (dim.label())
                dim_label = *dim.label();
            if (dim.unit())
                dim_label = dim_label + " [" + *dim.unit() + "]";
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            nix::RangeDimension dim = d.asRangeDimension();
            if (dim.label())
                dim_label = *dim.label();
            if (dim.unit())
                dim_label = dim_label + " [" + *dim.unit() + "]";
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            dim_label = "";
        } else {
            std::cerr << "unsupported dimension type" << std::endl;
        }
        labels.push_back(QString::fromStdString(dim_label));
    }
}


void Plotter::data_array_to_qvector(const nix::DataArray &array, QVector<double> &xdata, QVector<double> &ydata, QVector<QString> &xticklabels, nix::ndsize_t dim_index) {
    nix::Dimension d = array.getDimension(dim_index);

    if (d.dimensionType() == nix::DimensionType::Sample) {
        nix::SampledDimension dim = d.asSampledDimension();
        std::vector<double> ax = dim.axis(array.dataExtent()[0]);
        xdata = QVector<double>::fromStdVector(ax);

        std::vector<double> data(ax.size());
        array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
        ydata = QVector<double>::fromStdVector(data);
    } else if (d.dimensionType() == nix::DimensionType::Range) {
        nix::RangeDimension dim = d.asRangeDimension();
        std::vector<double> ax = dim.axis(array.dataExtent()[0]);
        xdata = QVector<double>::fromStdVector(ax);

        std::vector<double> data(ax.size());
        array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
        ydata = QVector<double>::fromStdVector(data);
        if (dim.alias()) {
            ydata.fill(1.0);
        }
    } else if (d.dimensionType() == nix::DimensionType::Set) {
        std::vector<double> data;
        array.getData(data);
        ydata = QVector<double>::fromStdVector(data);

        nix::SetDimension dim = d.asSetDimension();
        std::vector<std::string> labels = dim.labels();
        for (size_t i = 0; i < labels.size(); ++i) {
            xticklabels.push_back(QString::fromStdString(labels[i]));
            xdata.push_back(static_cast<double>(i));
        }
        if (labels.size() == 0) {
            for (int i = 0; i < ydata.size(); ++i) {
                xticklabels.push_back(QString::fromStdString(nix::util::numToStr<int>(i)));
                xdata.push_back(static_cast<double>(i));
            }
        }
     } else {
        std::cerr << "unsupported dimension type" << std::endl;
     }
}


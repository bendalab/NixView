#include "lineplotter.h"
#include "ui_lineplotter.h"

LinePlotter::LinePlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinePlotter)
{
    ui->setupUi(this);
}


LinePlotter::~LinePlotter()
{
    delete ui;
}


PlotterType LinePlotter::plotter_type() const {
    return PlotterType::Line;
}


void LinePlotter::set_label(const std::string &label) {
    ui->label->setText(QString::fromStdString(label));
}


void LinePlotter::set_xlabel(const QString &label){
    ui->plot->xAxis->setLabel(label);
    ui->plot->replot();
}


void LinePlotter::set_ylabel(const QString &label){
    ui->plot->yAxis->setLabel(label);
    ui->plot->replot();
}


void LinePlotter::add_line_plot(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name) {
    ui->plot->addGraph();
    ui->plot->graph()->addData(x_data, y_data);
    ui->plot->xAxis->setRange(x_data[0], x_data.last());
    double y_min = *std::min_element(std::begin(y_data), std::end(y_data));
    double y_max = *std::max_element(std::begin(y_data), std::end(y_data));
    if (y_min == y_max)
        y_min = 0.0;
    ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    ui->plot->graph()->setName(name);
    ui->plot->replot();
}


void LinePlotter::add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) {
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


void LinePlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {
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

QCustomPlot* LinePlotter::get_plot() {
    return ui->plot;
}

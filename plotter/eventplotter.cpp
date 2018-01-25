#include "eventplotter.h"
#include "ui_eventplotter.h"

EventPlotter::EventPlotter(QWidget *parent) :
  QWidget(parent), ui(new Ui::EventPlotter) {
    ui->setupUi(this);
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->plot, SIGNAL(selectionChangedByUser()), this, SLOT(selection_changed()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mouse_press()));
    connect(ui->plot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouse_wheel()));
    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));
    // setup policy and connect slot for context menu popup:
    ui->plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
}

EventPlotter::~EventPlotter() {
    delete ui;
}

PlotterType EventPlotter::plotter_type() const {
    return PlotterType::Event;
}

void EventPlotter::set_label(const std::string &label) {
    //ui->label->setText(QString::fromStdString(label));
}

// To be routed to the x-Axis Plotter
void EventPlotter::set_xlabel(const std::string &label) {
    set_xlabel(QString::fromStdString(label));
}

void EventPlotter::set_xlabel(const QString &label){
    ui->plot->xAxis->setLabel(label);
    ui->plot->replot();
}


void EventPlotter::set_ylabel(const std::string &label) {
    set_ylabel(QString::fromStdString(label));
}


void EventPlotter::set_ylabel(const QString &label){
    ui->plot->yAxis->setLabel(label);
    ui->plot->replot();
}

void EventPlotter::add_events(const QVector<double> &x_data, const QString &name, bool y_scale) {

}

void EventPlotter::add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) {

}

void EventPlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {

}

QCustomPlot* EventPlotter::get_plot() {
    return NULL;
}


void EventPlotter::draw(const nix::DataArray &array) {
    if (array.dimensionCount() > 1) {
        std::cerr << "EventPlotter::draw cannot draw 2 or more dimensional data!" << std::endl;
        return;
    }

    if(! Plotter::check_plottable_dtype(array)) {
        std::cerr << "Eventplotter::draw cannot handle that data type." << std::endl;
        return;
    }

    if(array.getDimension(1).dimensionType() != nix::DimensionType::Range) {
        std::cerr << "Eventplotter::draw(nix::DataArray) cannot handle datatypes other than range." << std::endl;
        return;
    }

    //draw that array!

}

























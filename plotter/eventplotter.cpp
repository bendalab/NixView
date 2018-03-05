#include "eventplotter.h"
#include "ui_eventplotter.h"

EventPlotter::EventPlotter(QWidget *parent) :
  QWidget(parent), ui(new Ui::EventPlotter) {
    ui->setupUi(this);
    // connect slot that ties some axis selections together (especially opposite axes):
    //connect(ui->plot, SIGNAL(selectionChangedByUser()), this, SLOT(selection_changed()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    //connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mouse_press()));
    //connect(ui->plot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouse_wheel()));
    // make bottom and left axes transfer their ranges to top and right axes:
    //connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));
    // setup policy and connect slot for context menu popup:
    ui->plot->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(ui->plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);

    ui->plot->axisRect()->setRangeZoom(ui->plot->xAxis->orientation());
    ui->plot->axisRect()->setRangeDrag(ui->plot->xAxis->orientation());
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


void EventPlotter::add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) {
    plot(x_data);
}


void EventPlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {
    plot(positions, extents);
}


QCustomPlot* EventPlotter::get_plot() {
    return ui->plot;
}


void EventPlotter::draw(const QVector<double> &positions, const QString &ylabel, const QVector<QString> &xlabels) {
    set_xlabel(xlabels[0]);
    set_ylabel(ylabel);

    plot(positions);
}


void EventPlotter::plot(const QVector<double> &positions) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);

    QVector<double> xValues = QVector<double>(4*positions.size());
    QVector<double> yValues = QVector<double>(4*positions.size());

    for(int i = 0; i < positions.size(); i++) {
        xValues[4*i]   = positions[i]-(1.0/80000); // half of a step of the max(?) sample rate (40k).
        xValues[4*i+1] = positions[i];
        xValues[4*i+2] = positions[i];
        xValues[4*i+3] = positions[i]+(1.0/80000);

        yValues[4*i]   = 0;
        yValues[4*i+1] = 1;
        yValues[4*i+2] = -1;
        yValues[4*i+3] = 0;
    }

    ui->plot->graph()->addData(xValues, yValues);
    ui->plot->xAxis->setRange(xValues[0], xValues.last());
    ui->plot->yAxis->setRange(-1,1);
    ui->plot->replot();
}

void EventPlotter::draw(const QVector<double> &positions, const QVector<double> &extents, const QString &ylabel, const QVector<QString> &xlabels) {
    set_xlabel(xlabels[0]);
    set_ylabel(ylabel);

    plot(positions, extents);
}

void EventPlotter::plot(const QVector<double> &positions, const QVector<double> &extends) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);

    QVector<double> xValues = QVector<double>(4*positions.size());
    QVector<double> yValues = QVector<double>(4*positions.size());

    for(int i = 0; i < positions.size(); i++) {
        xValues[4*i]   = positions[i]-(1.0/80000);
        xValues[4*i+1] = positions[i];
        xValues[4*i+2] = positions[i] + extends[i];
        xValues[4*i+3] = positions[i] + extends[i]+(1.0/80000);

        yValues[4*i]   = 0;
        yValues[4*i+1] = 1;
        yValues[4*i+2] = 1;
        yValues[4*i+3] = 0;
    }

    ui->plot->graph()->addData(xValues, yValues);
    ui->plot->xAxis->setRange(xValues[0], xValues.last());
    ui->plot->yAxis->setRange(-1.1,1.1);
    ui->plot->replot();
}


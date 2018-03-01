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

}

void EventPlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {

}

QCustomPlot* EventPlotter::get_plot() {
    return ui->plot;
}


void EventPlotter::draw(const QVector<QVector<double>> &positions, const QVector<QVector<QString>> &name) {
//    if(! checkDimensions(positions)) {
//        return;
//    }

    for (int i = 0; i < positions.size(); i++) {
        plot(positions[i]);
    }
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

void EventPlotter::draw(const QVector<QVector<double>> &positions, const QVector<QVector<double>> &extends, const QVector<QVector<QString>> &name) {
//    if( ! checkDimensions(positions, extends)) {
//        return;
//    }

    for (int i = 0; i < positions.size(); i++) {
        plot(positions[i], extends[i]);
    }
}

void EventPlotter::plot(const QVector<double> &positions, const QVector<double> &extends) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);

    QVector<double> xValues = QVector<double>(4*positions.size());
    QVector<double> yValues = QVector<double>(4*positions.size());

    for(int i = 0; i < positions.size(); i++) {
        xValues[4*i]   = positions[i];
        xValues[4*i+1] = positions[i];
        xValues[4*i+2] = positions[i] + extends[i];
        xValues[4*i+3] = positions[i] + extends[i];

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



//bool EventPlotter::checkDimensions(QVector<QVector<double>> &positions, QVector<QVector<double>> &extends) {


//    if(extends.dataExtent().size() == positions.dataExtent().size() && positions.dataExtent().size() > 2) {
//        std::cerr << "Eventplotter::draw positions and extends dont have the same number of dims or more than 2." << std::endl;
//        return false;
//    }

//    for(unsigned int i = 0; i < positions.size(); i++) {
//        if(positions[i].size() != extends[i].size()) {
//            std::cerr << "Eventplotter::draw positions and extends dont always have the same length." << std::endl;
//            return false;
//        }
//    }

//    if(! Plotter::check_plottable_dtype(positions) || ! Plotter::check_plottable_dtype(extends)) {
//        std::cerr << "Eventplotter::draw cannot handle that data type. (extends)" << std::endl;
//        return false;
//    }

//    return true;
//}

//bool EventPlotter::checkDimensions(const nix::DataArray &positions) {
//    if(positions.dataExtent().size() > 2) {
//        std::cerr << "Eventplotter::draw positions has more than 2 dimensions." << std::endl;
//        return false;
//    }
//    if(! Plotter::check_plottable_dtype(positions)) {
//        std::cerr << "Eventplotter::draw cannot handle that data type. (positions)" << std::endl;
//        return false;
//    }

//    return true;
//}








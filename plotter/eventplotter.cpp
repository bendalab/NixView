#include "eventplotter.h"
#include "ui_eventplotter.h"

EventPlotter::EventPlotter(QWidget *parent, int numOfPoints) :
  QWidget(parent), ui(new Ui::EventPlotter), thread() {
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

    qRegisterMetaType<QVector<double>>();

    ui->plot->yAxis->setRange(-0.05,1.05);
    this->numOfPoints = numOfPoints;
    thread.setChuncksize(static_cast<unsigned int>(numOfPoints));
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


void EventPlotter::draw(const nix::DataArray &array) {
    if(! testArray(array)) {
        return;
    }

    this->array = array;

    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);
    ui->plot->graph()->setLineStyle(QCPGraph::LineStyle::lsImpulse);

    set_ylabel(array.name());

    nix::NDSize start(1), extent(1);
    start[0] = 0;
    unsigned int length = numOfPoints;
    if(array.dataExtent()[0] < length) {
        length = array.dataExtent()[0];
    }
        extent[0] = length;

    nix::Dimension d = array.getDimension(1);

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xRangeChanged(QCPRange)));
    this->totalRange.expand(QCPRange(d.asRangeDimension().axis(1,0)[0], d.asRangeDimension().axis(1,array.dataExtent()[0]-1)[0]));
    ui->plot->xAxis->setRange(QCPRange(d.asRangeDimension().axis(1,0)[0],d.asRangeDimension().axis(1,length-1)[0]));

    connect(&thread, SIGNAL(dataReady(const QVector<double> &, const QVector<double> &, int)), this, SLOT(drawThreadData(const QVector<double> &, const QVector<double> &, int)));
    thread.setVariables1D(array, start, extent, array.getDimension(1), 0 );

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xRangeChanged(QCPRange)));
}


bool EventPlotter::testArray(const nix::DataArray &array) {
    if(array.dataExtent().size() > 1) {
        std::cerr << "Eventplotter::testArray() - Eventplotter can't handle 2D data." << std::endl;
        return false;
    }

    if(array.dataExtent().size() == 0) {
        std::cerr << "Eventplotter::testArray() - array with dimCount: 0." << std::endl;
        return false;
    }

    nix::Dimension d = array.getDimension(1);
    if(d.dimensionType() != nix::DimensionType::Range) {
        std::cerr << "Eventplotter::testArray() - Eventplotter can't handle other dimensions than alias ranged." << std::endl;
        return false;
    }
    if( ! d.asRangeDimension().alias()) {
        std::cerr << "Eventplotter::testArray() - Eventplotter ranged dimension is not an alias ranged." << std::endl;
        return false;
    }
    return true;
}


void EventPlotter::draw(const QVector<double> &positions, const QString &ylabel, const QVector<QString> &xlabels) {
    if(positions.size() == 0) {
        return;
    }
    set_xlabel(xlabels[0]);
    set_ylabel(ylabel);

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xRangeChanged(QCPRange)));
    this->totalRange.expand(QCPRange(positions.first(), positions.last()));
    ui->plot->xAxis->setRange(positions.first(), positions.last());

    plot(positions);
}


void EventPlotter::plot(const QVector<double> &positions) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);
    ui->plot->graph()->setLineStyle(QCPGraph::LineStyle::lsImpulse);

    QVector<double> yValues(positions.size());
    yValues.fill(1);

    ui->plot->graph()->addData(positions, yValues, true);
    ui->plot->xAxis->setRange(positions[0], positions.last());
    ui->plot->replot();
}

void EventPlotter::draw(const QVector<double> &positions, const QVector<double> &extents, const QString &ylabel, const QVector<QString> &xlabels) {
    if(positions.size() == 0) {
        return;
    }
    set_xlabel(xlabels[0]);
    set_ylabel(ylabel);

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xRangeChanged(QCPRange)));
    this->totalRange.expand(QCPRange(positions.first(), positions.last()+extents.last()));
    ui->plot->xAxis->setRange(positions.first(), positions.last()+extents.last());

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

    ui->plot->graph()->addData(xValues, yValues, true);
    ui->plot->xAxis->setRange(xValues[0], xValues.last());
    ui->plot->replot();
}


void EventPlotter::drawThreadData(const QVector<double> &positions, const QVector<double> &axis, int graphIndex) {

    QVector<double> yValues(positions.size());
    yValues.fill(1);

    ui->plot->graph(graphIndex)->setData(positions, yValues, true);
    ui->plot->replot();
}

void EventPlotter::xRangeChanged(QCPRange newRange) {
    //assumption has exactly one graph.
    emit xAxisChanged(newRange, totalRange); // signal for scrollbar and zoom.

    QCPGraph *graph = ui->plot->graph();

    if(graph->dataCount() == 0) {
        return;
    }

    double max = graph->dataMainKey(graph->dataCount()-1);
    double min = graph->dataMainKey(0);
    double meanPoints = graph->dataCount() / (max-min);

    thread.startLoadingIfNeeded(newRange,1, min, max, meanPoints);
}

void EventPlotter::changeXAxisPosition(double newCenter) {
    if(ui->plot->xAxis->range().center() != newCenter) {
        ui->plot->xAxis->setRange(newCenter, ui->plot->xAxis->range().size(), Qt::AlignCenter);
        ui->plot->replot();
    }
}

void EventPlotter::changeXAxisSize(double ratio) {
    double xNewSize = totalRange.size() * ratio;

    if(xNewSize != ui->plot->xAxis->range().size()) {
        ui->plot->xAxis->setRange(ui->plot->xAxis->range().center(), xNewSize, Qt::AlignCenter);
        ui->plot->replot();
    }
}

void EventPlotter::resetView() {
    QCPDataContainer<QCPGraphData> data = *ui->plot->graph()->data().data();

    // reset x Range
    if(numOfPoints != 0 && numOfPoints < data.size()) {
        QCPGraphData firstPoint = *data.at(0);
        QCPGraphData lastPoint = *data.at(numOfPoints);
        QCPRange resetX = QCPRange(firstPoint.sortKey(), lastPoint.sortKey());
        ui->plot->xAxis->setRange(resetX);
    } else {
        ui->plot->xAxis->setRange(totalRange);
    }
}


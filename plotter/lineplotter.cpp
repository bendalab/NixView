#include "lineplotter.h"
#include "ui_lineplotter.h"
#include <QMenu>

LinePlotter::LinePlotter(QWidget *parent, int numOfPoints) :
    QWidget(parent), ui(new Ui::LinePlotter), cmap(), totalXRange(0,0), totalYRange(0,0) {
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

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisNewRange(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(yAxisNewRange(QCPRange)));

    qRegisterMetaType<QVector<double>>();
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(testThreads(QCPRange)));

    this->numOfPoints = numOfPoints; // standard 100 000
    this->totalYRange = QCPRange(0, 0);
}

LinePlotter::~LinePlotter()
{
    for (int i=loaders.size()-1; i >= 0; i--) {
        delete loaders[i];
    }

    delete ui;

}


PlotterType LinePlotter::plotter_type() const {
    return PlotterType::Line;
}


void LinePlotter::set_label(const std::string &label) {
    //ui->label->setText(QString::fromStdString(label));
}


void LinePlotter::set_xlabel(const std::string &label) {
    set_xlabel(QString::fromStdString(label));
}


void LinePlotter::set_xlabel(const QString &label){
    ui->plot->xAxis->setLabel(label);
    ui->plot->replot();
}


void LinePlotter::set_ylabel(const std::string &label) {
    set_ylabel(QString::fromStdString(label));
}


void LinePlotter::set_ylabel(const QString &label){
    ui->plot->yAxis->setLabel(label);
    ui->plot->replot();
}


void LinePlotter::draw(const nix::DataArray &array) {
    if (!Plotter::check_plottable_dtype(array)) {
        std::cerr << "LinePlotter::draw cannot handle data type " << array.dataType() << std::endl;
        return;
    }
    if (!check_dimensions(array)) {
        std::cerr << "LinePlotter::draw cannot handle dimensionality of the data" << std::endl;
        return;
    }

    arrays.append(array);
    loaders.append(new LoadThread());

    LoadThread *loader = loaders.last();

    connect(loader, SIGNAL(dataReady(const QVector<double> &, const QVector<double> &, int)), this, SLOT(drawThreadData(const QVector<double> &, const QVector<double> &, int)));
    //connect(loader, SIGNAL(progress(double)), this, SLOT(printProgress(double)));

    if (array.dataExtent().size() == 1) {
        draw_1d(array);
    } else {
        draw_2d(array);
    }
}


void LinePlotter::draw_1d(const nix::DataArray &array) {
    nix::Dimension d = array.getDimension(1);

    QString y_label;
    QVector<QString> ax_labels;
    data_array_ax_labels(array, y_label, ax_labels);

    this->set_ylabel(y_label);
    this->set_xlabel(ax_labels[0]);
    this->set_label(array.name());

    if(d.dimensionType() == nix::DimensionType::Set) {
        QVector<double> x_axis, y_axis;
        QVector<QString> x_tick_labels;
        data_array_to_qvector(array, x_axis, y_axis, x_tick_labels, 1);

        // plot 1d set data in a meaningfull way.
        //   ?? this->add_line_plot(x_axis, y_axis, QString::fromStdString(array.name()));

    } else {
        int newGraphIndex = ui->plot->graphCount();

        expandXRange(array, 1);

        ui->plot->addGraph();
        ui->plot->graph()->setPen(QPen(cmap.next()));
        nix::NDSize start(1);
        start[0] = 0;

        nix::NDSize extent(1);
        extent[0] = numOfPoints;

        loaders.last()->setVariables1D(array, start, extent, array.getDimension(1), newGraphIndex);

        // open loading dialog ? too fast for small amounts (TODO: general loading Dialog)
    }
}


void LinePlotter::draw_2d(const nix::DataArray &array) {

    /*
    QVector<double> x_axis, y_axis;
    QVector<QString> labels;
    get_data_array_axis(array, x_axis, labels, best_dim);
    get_data_array_axis(array, y_axis, labels, 3-best_dim);
    for (int i = 0; i < y_axis.size(); i++) {
        QVector<double> data = get_data_line(array, i, best_dim);
        add_line_plot(x_axis, data, labels[i]);
    }
    QString y_label;
    QVector<QString> ax_labels;
    data_array_ax_labels(array, y_label, ax_labels);
    this->set_ylabel(y_label);
    this->set_xlabel(ax_labels[best_dim-1]);
    */

    int best_dim = guess_best_xdim(array);
    int firstGraphIndex = ui->plot->graphCount();

    expandXRange(array, best_dim);

    for(unsigned int i=0; i<array.dataExtent()[2-best_dim]; i++) {
        QPen pen;
        pen.setColor(cmap.next());

        ui->plot->addGraph();
        ui->plot->graph()->setPen(pen);
    }

    nix::NDSize start(2);
    start[0] = 0;
    start[1] = 0;

    double length = array.dataExtent()[best_dim-1];
    if(numOfPoints < length) {
        length = numOfPoints;
    }
    nix::NDSize extent(2);
    extent[best_dim-1] = length;
    extent[2-best_dim] = 1;

    loaders.last()->setVariables(array, start, extent, array.getDimension(best_dim), std::vector<int>(), best_dim, firstGraphIndex);
}

/*
void LinePlotter::calcStartExtent(const nix::DataArray &array, nix::NDSize &start_size, nix::NDSize& extent_size, int xDim) {
    QCPRange curRange = ui->plot->xAxis->range();
    nix::Dimension d = array.getDimension(xDim);

    double start, extent;

    if( d.dimensionType() == nix::DimensionType::Set) {
        start = 0;
        extent = array.dataExtent()[xDim-1];
    } else {
        double pInRange;
        double startIndex;

        if( d.dimensionType() == nix::DimensionType::Sample) {
            nix::SampledDimension spd = d.asSampledDimension();
            double samplingIntervall = spd.samplingInterval();
            double offset = 0;
            if(spd.offset()) {
                offset = spd.offset().get();
            }

            startIndex = (curRange.lower - offset) / samplingIntervall;
            pInRange = ui->plot->xAxis->range().size() / samplingIntervall;

        } else { // rangeDimension
            nix::RangeDimension rd = d.asRangeDimension();
            std::vector<double> ticks = rd.ticks();
            startIndex = std::distance(ticks.cbegin(), std::lower_bound(ticks.cbegin(), ticks.cend(), curRange.lower));
            pInRange   = std::distance(ticks.cbegin(), std::upper_bound(ticks.cbegin(), ticks.cend(), curRange.upper)) - startIndex;
        }

        if(pInRange <= numOfPoints) {
            start  = startIndex - numOfPoints;
            extent =  numOfPoints + pInRange + numOfPoints;

        } else if (pInRange > numOfPoints && pInRange < numOfPoints * 3) {
            start  = startIndex - numOfPoints / 2;
            extent = pInRange + numOfPoints; //numOfPoints/2 + pInRange + numOfPoints/2
        } else {
            start  = startIndex;
            extent = pInRange + 1;
        }
    }

    start = std::floor(start);
    extent = std::ceil(extent);

    if(start < 0) {
        start = 0;
    }

    if(extent > array.dataExtent()[xDim-1] - start) {
        extent = array.dataExtent()[xDim-1] - start;
    } else if(extent < 1) {
        extent = 1;
    }

    if(array.dataExtent().size() == 1) {
        start_size = nix::NDSize({static_cast<int>(start)});
        extent_size = nix::NDSize({static_cast<int>(extent)});
    } else {
        start_size = nix::NDSize({0, 0});
        start_size[xDim-1] = static_cast<int>(start);
        extent_size = nix::NDSize({1,1});
        extent_size[xDim-1] = static_cast<int>(extent);
    }

    //std::cerr << "start: " << start << std::endl;
    //std::cerr << "extent: " << extent << std::endl;
}
*/

int LinePlotter::guess_best_xdim(const nix::DataArray &array) const {

    if(array.dataExtent().size() == 0) {
        throw nix::IncompatibleDimensions("Array has dataExtent().size 0.", "guess_best_xdim");
    }

    if(array.dataExtent().size() > 2) {
        throw nix::IncompatibleDimensions("Array has more than two dimensions.", "guess_best_xdim");
    }

    if(array.dataExtent().size() == 1) {
        return 1;
    } else { //(array.dataExtent().size() == 2) {
        nix::DimensionType d_1 = array.getDimension(1).dimensionType();
        nix::DimensionType d_2 = array.getDimension(2).dimensionType();

        if(d_1 == nix::DimensionType::Sample) {
            return 1;
        } else if (d_2 == nix::DimensionType::Sample) {
            return 2;
        } else {
            if(d_1 == nix::DimensionType::Set && d_2 == nix::DimensionType::Range) {
                return 2;
            } else if (d_1 == nix::DimensionType::Range && d_2 == nix::DimensionType::Set){
                return 1;
            } else {
                std::cerr << "How did you get with 2D Set Data to guess_best_xdims() in the Lineplotter?" << std::endl;
                throw nix::IncompatibleDimensions("Array contains 2D set data.", "guess_best_xdim");
            }
        }
    }
}


bool LinePlotter::check_dimensions(const nix::DataArray &array) const {
    if (array.dataExtent().size() > 2) {
        std::cerr << "LinePlotter::check_dimensions cannot draw 3D!" << std::endl;
        return false;
    }

    if (array.dataExtent().size() == 0 || array.dataExtent().size() > 2) {
        return false;
    }

    if(array.dataExtent().size() == 1) {
        return true;
    }

    nix::DimensionType dt_1 = array.getDimension(1).dimensionType();
    nix::DimensionType dt_2 = array.getDimension(2).dimensionType();

    if ((dt_1 == nix::DimensionType::Sample || dt_1 == nix::DimensionType::Range) && dt_2 == nix::DimensionType::Set) {
        return true;
    }

    if (dt_1 == nix::DimensionType::Set && (dt_2 == nix::DimensionType::Range || dt_2 == nix::DimensionType::Sample)) {
        return true;
    }
    if (dt_1 == nix::DimensionType::Set && dt_2 == nix::DimensionType::Set) {
        std::cerr << "LinePlotter should draw 2D Set data? You serious?" << std::endl;
        return false;
    }
    return false;
}


void LinePlotter::add_line_plot(const QVector<double> &xData, const QVector<double> &yData, const QString &name) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);
    ui->plot->graph()->addData(xData, yData);

    setXRange(xData);
    setYRange(yData);

    ui->plot->graph()->setName(name);
    ui->plot->replot();
}


void LinePlotter::add_events(const QVector<double> &xData, const QVector<double> &yData, const QString &name, bool yScale) {
    ui->plot->addGraph();
    ui->plot->graph()->addData(xData, yData);
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->plot->graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
    ui->plot->graph()->setName(name);
    ui->plot->graph()->setPen(QPen(Qt::red));
    if (yScale) {
        setYRange(yData);
    }
    setXRange(xData);
}


void LinePlotter::add_events(const QVector<double> &xData, const QString &name, bool yScale) {
    double max = ui->plot->yAxis->range().upper * 0.9;
    QVector<double> yData(xData.size(), max);
    add_events(xData, yData, name, yScale);
}


void LinePlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {
    if (extents.size() > 0 && (positions.size() != extents.size())) {
        std::cerr << "Cannot draw segments, number of positions and extents does not match!" << std::endl;
        return;
    }
    for (int i = 0; i<positions.size(); i++) {
        QCPItemRect *rect = new QCPItemRect(ui->plot);
        double x_min, x_max, y_min, y_max;
        y_max = ui->plot->yAxis->range().upper;
        y_min = ui->plot->yAxis->range().lower;
        x_min = positions[i];
        if (extents.size() > 0)
            x_max = x_min + extents[i];
        else
            x_max = positions[i];

        rect->position("topLeft")->setCoords(x_min, y_max);
        rect->position("bottomRight")->setCoords(x_max, y_min);
        rect->setPen(QPen(Qt::red));
        rect->setBrush(QBrush(QColor(255, 10, 10, 50)));
    }
}


QCustomPlot* LinePlotter::get_plot() {
    return ui->plot;
}

void LinePlotter::expandXRange(const nix::DataArray &array, int xDim) {
    int dimI = xDim-1;

    int maxLoad = numOfPoints;
    if(array.dataExtent()[dimI] < static_cast<unsigned>(numOfPoints)) {
        maxLoad = array.dataExtent()[dimI];
    }

    nix::Dimension d = array.getDimension(xDim);
    unsigned int dimMax = array.dataExtent()[dimI];

    if(d.dimensionType() == nix::DimensionType::Sample) {
        totalXRange.expand(QCPRange(d.asSampledDimension().axis(1,0)[0], d.asSampledDimension().axis(1,dimMax-1)[0]));
        ui->plot->xAxis->setRange(d.asSampledDimension().axis(1,0)[0], d.asSampledDimension().axis(1,maxLoad-1)[0]);
    } else if (d.dimensionType() == nix::DimensionType::Range) {
        totalXRange.expand(QCPRange(d.asRangeDimension().axis(1)[0],d.asRangeDimension().axis(1,dimMax-1)[0]));
        ui->plot->xAxis->setRange(QCPRange(d.asRangeDimension().axis(1,0)[0],d.asRangeDimension().axis(1,maxLoad-1)[0]));
    } else {
        // How does this work for set dim ? TODO
        std::cerr << "Lineplotter::setXRange(array), not yet done. " << std::endl;
    }
}


void LinePlotter::setXRange(QVector<double> xData) {
    totalXRange.expand(QCPRange(xData[0],xData.last()));

    if(numOfPoints < xData.size() && numOfPoints != 0) {
        ui->plot->xAxis->setRange(xData[0], xData[numOfPoints]);
    } else {
        ui->plot->xAxis->setRange(totalXRange);
    }
}


void LinePlotter::expandYRange(QVector<double> yData) {
    double yMin = *std::min_element(yData.constBegin(), yData.constEnd());
    double yMax = *std::max_element(yData.constBegin(), yData.constEnd());
    if (yMin == yMax)
        yMin = yMax-1;

    totalYRange.expand(QCPRange(yMin, yMax));
}


void LinePlotter::setYRange(QVector<double> yData) {
    double yMin = *std::min_element(yData.constBegin(), yData.constEnd());
    double yMax = *std::max_element(yData.constBegin(), yData.constEnd());
    if (yMin == yMax)
        yMin = yMax-1;

    totalYRange.expand(QCPRange(yMin, yMax));
    ui->plot->yAxis->setRange(totalYRange.lower*1.05, totalYRange.upper*1.05);

    //emit yAxisNewRange(ui->plot->yAxis->range());
}


void LinePlotter::drawThreadData(const QVector<double> &data, const QVector<double> &axis, int graphIndex) {
    if(totalYRange == QCPRange(0, 0)) {
        setYRange(data);
    } else {
        expandYRange(data);
    }
    ui->plot->graph(graphIndex)->setData(axis, data, true);
    ui->plot->replot();
}

void LinePlotter::printProgress(double progress) {
    std::cerr << "Loaded: " << progress*100 << "%" << std::endl;
}

void LinePlotter::resetView() {
    QCPDataContainer<QCPGraphData> data = *ui->plot->graph()->data().data();

    // reset x Range
    if(numOfPoints != 0 && numOfPoints < data.size()) {
        QCPGraphData firstPoint = *data.at(0);
        QCPGraphData lastPoint = *data.at(numOfPoints);
        QCPRange resetX = QCPRange(firstPoint.sortKey(), lastPoint.sortKey());
        ui->plot->xAxis->setRange(resetX);
    } else {
        ui->plot->xAxis->setRange(totalXRange);
    }

    ui->plot->yAxis->setRange(totalYRange.lower*1.05, totalYRange.upper*1.05);
}


void LinePlotter::testThreads(QCPRange range) {
    if(ui->plot->graphCount() == 0) {
        return;
    }

    int graphIndex = 0;
    for(int i=0; i<arrays.size(); i++) {
        int xDim = guess_best_xdim(arrays[i]);
        QCPGraph *graph = ui->plot->graph(graphIndex);

        if(graph->dataCount() == 0) {
            // with setVariables ?
            //loaders[i]->setVariables();

            if(arrays[i].dataExtent().size() == 1) {
                graphIndex += 1;
            } else if(arrays[i].dataExtent().size() == 2) {
                graphIndex += arrays[i].dataExtent()[2-xDim];
            }
            continue;
        }

        double max = graph->dataMainKey(graph->dataCount()-1);
        double min = graph->dataMainKey(0);
        double mean = graph->dataCount() / (max-min);

        loaders[i]->startLoadingIfNeeded(range, xDim, min, max, mean);

        if(arrays[i].dataExtent().size() == 1) {
            graphIndex += 1;
        } else if(arrays[i].dataExtent().size() == 2) {
            graphIndex += arrays[i].dataExtent()[2-xDim];
        }
    }
}

/*
void LinePlotter::checkGraphsPerArray(QCPRange range) {
    if(ui->plot->graphCount() == 0) {
        return;
    }

    int graphIndex = 0;
    // TODO: FEATURE plot only some graphs of a DataArray
    for(int i=0; i<arrays.size(); i++) {
        QCPGraph *graph = ui->plot->graph(graphIndex);

        if(graph->dataCount() == 0) {
            nix::NDSize start, extent;
            int xDim = guess_best_xdim(arrays[i]);

            calcStartExtent(arrays[i], start, extent, xDim);
            loaders[i]->setVariables(arrays[i], start, extent, arrays[i].getDimension(xDim), std::vector<int>(), xDim, graphIndex);

            if(arrays[i].dataExtent().size() == 1) {
                graphIndex += 1;
            } else if(arrays[i].dataExtent().size() == 2) {
                graphIndex += arrays[i].dataExtent()[2-xDim];
            }
            continue;
        }

        double max = graph->dataMainKey(graph->dataCount()-1);
        double min = graph->dataMainKey(0);
        double mean = graph->dataCount() / (max-min);

        int xDim = guess_best_xdim(arrays[i]);

        if((range.lower - min)*mean < numOfPoints/4) {
            if(checkForMoreData(i, min, false)) {
                nix::NDSize start, extent;
                calcStartExtent(arrays[i], start, extent, xDim);

                loaders[i]->setVariables(arrays[i], start, extent, arrays[i].getDimension(xDim), std::vector<int>(), xDim, graphIndex);
            }
        }
        if((max - range.upper) * mean < numOfPoints / 4) {
            if(checkForMoreData(i, max, true)) {
                nix::NDSize start, extent;
                calcStartExtent(arrays[i], start, extent, xDim);

                loaders[i]->setVariables(arrays[i], start, extent, arrays[i].getDimension(xDim), std::vector<int>(), xDim, graphIndex);
            }
        }

        if(arrays[i].dataExtent().size() == 1) {
            graphIndex += 1;
        } else if(arrays[i].dataExtent().size() == 2) {
            graphIndex += arrays[i].dataExtent()[2-xDim];
        }
    }
}


bool LinePlotter::checkForMoreData(int arrayIndex, double currentExtreme, bool higher) {
    nix::DataArray array = arrays[arrayIndex];

    int xDim = guess_best_xdim(array);
    nix::Dimension d = array.getDimension(xDim);

    if(d.dimensionType() == nix::DimensionType::Set) {
        std::cerr << "Lineplotter::CheckForMoreData(): check set dim... no! Not yet." << std::endl;
        return false;
    } else if(d.dimensionType() == nix::DimensionType::Sample) {
        if(higher) {
            return (d.asSampledDimension().axis(1,array.dataExtent()[xDim-1]-1)[0] > currentExtreme);
        } else {
            return (d.asSampledDimension().axis(1,0)[0] < currentExtreme);
        }
    } else if(d.dimensionType() == nix::DimensionType::Range) {
        if(higher) {
            return (d.asRangeDimension().axis(1,array.dataExtent()[xDim-1]-1)[0] > currentExtreme);
        } else {
            return (d.asRangeDimension().axis(1,0)[0] < currentExtreme);
        }
    } else {
        std::cerr << "Lineplotter::CheckForMoreData(): unsupported dimension type." << std::endl;
        return false;
    }
}
*/


void LinePlotter::xAxisNewRange(QCPRange range) {
    emit xAxisChanged(range,totalXRange);
}

void LinePlotter::yAxisNewRange(QCPRange range) {
    emit yAxisChanged(range, totalYRange);
}

void LinePlotter::changeXAxisPosition(double newCenter) {
    if(ui->plot->xAxis->range().center() != newCenter) {
        ui->plot->xAxis->setRange(newCenter, ui->plot->xAxis->range().size(), Qt::AlignCenter);
        ui->plot->replot();
    }
}

void LinePlotter::changeYAxisPosition(double newCenter) {
    if(ui->plot->yAxis->range().center() != newCenter) {
        ui->plot->yAxis->setRange(newCenter, ui->plot->yAxis->range().size(), Qt::AlignCenter);
        ui->plot->replot();
    }
}

void LinePlotter::changeXAxisSize(double ratio) {
    double xNewSize = totalXRange.size() * ratio;

    if(xNewSize != ui->plot->xAxis->range().size()) {
        ui->plot->xAxis->setRange(ui->plot->xAxis->range().center(), xNewSize, Qt::AlignCenter);
        ui->plot->replot();
    }
}


void LinePlotter::selection_changed() {
    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxisLabel) || ui->plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
            ui->plot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) || ui->plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
    {
        ui->plot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
        ui->plot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxisLabel) || ui->plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
            ui->plot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) || ui->plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
    {
        ui->plot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
        ui->plot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels|QCPAxis::spAxisLabel);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->plot->graphCount(); ++i) {
        QCPGraph *graph = ui->plot->graph(i);
        QCPPlottableLegendItem *item = ui->plot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected()) {
            item->setSelected(true);
            QCPDataRange wholeGraph = QCPDataRange(0, graph->dataCount());
            QCPDataSelection selection = QCPDataSelection(wholeGraph);
            graph->setSelection(selection);
        }
    }
}


void LinePlotter::mouse_press() {
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeDrag(ui->plot->xAxis->orientation());
    else if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeDrag(ui->plot->yAxis->orientation());
    else
        ui->plot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}


void LinePlotter::mouse_wheel() {
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeZoom(ui->plot->xAxis->orientation());
    else if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeZoom(ui->plot->yAxis->orientation());
    else
        ui->plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);

}


void LinePlotter::context_menu_request(QPoint pos) {
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    QAction *legend_action = menu->addAction("Show legend", this, SLOT(show_legend()));
    legend_action->setCheckable(true);
    legend_action->setChecked(ui->plot->legend->visible());
    menu->addAction("Clear selection", this, SLOT(clear_selection()));
    if (ui->plot->selectedGraphs().size() > 0) {
        menu->addAction("Remove selected graph", this, SLOT(remove_selected_graph()));
        QMenu *line_style_menu = menu->addMenu("Line style");
        line_style_menu->addAction("none", this, SLOT(set_pen_none()));
        line_style_menu->addAction("solid", this, SLOT(set_pen_solid()));
        line_style_menu->addAction("dashed", this, SLOT(set_pen_dashed()));
        line_style_menu->addAction("dash-dotted", this, SLOT(set_pen_dashed()));
        line_style_menu->addAction("dotted", this, SLOT(set_pen_dashed()));

        QMenu *marker_menu = menu->addMenu("Marker");
        marker_menu->addAction("none", this, SLOT(marker_none()));
        marker_menu->addAction("circle", this, SLOT(marker_circle()));
        marker_menu->addAction("cross", this, SLOT(marker_cross()));
        marker_menu->addAction("dot", this, SLOT(marker_dot()));
        marker_menu->addAction("diamond", this, SLOT(marker_diamond()));
        marker_menu->addAction("plus", this, SLOT(marker_plus()));
        marker_menu->addAction("Square", this, SLOT(marker_square()));
    }
    menu->popup(ui->plot->mapToGlobal(pos));
}


void LinePlotter::remove_selected_graph() {
    if (ui->plot->selectedGraphs().size() > 0) {
        ui->plot->removeGraph(ui->plot->selectedGraphs().first());
        ui->plot->replot();
    }
}


void LinePlotter::show_legend() {
    ui->plot->legend->setVisible(!ui->plot->legend->visible());
    ui->plot->replot();
}


void LinePlotter::clear_selection(){
    ui->plot->deselectAll();
    ui->plot->replot();
}


void LinePlotter::set_marker(QString marker){
    QCPGraph *graph = ui->plot->selectedGraphs().first();
    if (marker == "none") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 0));
    } else if (marker == "circle") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    } else if (marker == "diamond") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 10));
    } else if (marker == "dot") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDot, 10));
    } else if (marker == "cross") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 10));
    } else if (marker == "square") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 10));
    } else if (marker == "plus") {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 10));
    } else {
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 10));
    }
    clear_selection();
}


void LinePlotter::marker_circle() {
    set_marker("circle");
}


void LinePlotter::marker_cross() {
    set_marker("cross");
}


void LinePlotter::marker_dot() {
    set_marker("dot");
}


void LinePlotter::marker_diamond() {
    set_marker("diamond");
}

void LinePlotter::marker_square() {
    set_marker("square");
}

void LinePlotter::marker_none(){
    set_marker("none");
}

void LinePlotter::marker_plus(){
    set_marker("plus");
}


void LinePlotter::set_pen_style(QString style) {
    QCPGraph *graph = ui->plot->selectedGraphs().first();
    QPen pen = graph->pen();
    if (style == "none"){
        pen.setStyle(Qt::PenStyle::NoPen);
    } else if (style == "solid") {
        pen.setStyle(Qt::PenStyle::SolidLine);
    } else if (style == "dashed") {
        pen.setStyle(Qt::PenStyle::DashLine);
    } else if (style == "dotted") {
        pen.setStyle(Qt::PenStyle::DotLine);
    } else if (style == "dashdot") {
        pen.setStyle(Qt::PenStyle::DashDotLine);
    } else {
        pen.setStyle(Qt::PenStyle::SolidLine);
    }
    graph->setPen(pen);
    clear_selection();
}


void LinePlotter::set_pen_none() {
    set_pen_style("none");
}


void LinePlotter::set_pen_solid() {
    set_pen_style("solid");
}


void LinePlotter::set_pen_dashed() {
    set_pen_style("dashed");
}


void LinePlotter::set_pen_dotted() {
    set_pen_style("dotted");
}


void LinePlotter::set_pen_dashdotted() {
    set_pen_style("dashdot");
}

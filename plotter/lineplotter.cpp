#include "lineplotter.h"
#include "ui_lineplotter.h"
#include <QMenu>

LinePlotter::LinePlotter(QWidget *parent) :
    QWidget(parent), ui(new Ui::LinePlotter), cmap() {
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


LinePlotter::~LinePlotter()
{
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
    if (array.dimensionCount() > 2) {
        std::cerr << "LinePlotter::draw cannot draw 3D!" << std::endl;
        return;
    }
    if (!Plotter::check_plottable_dtype(array)) {
        std::cerr << "LinePlotter::draw cannot handle data type " << array.dataType() << std::endl;
        return;
    }
    if (!check_dimensions(array)) {
        std::cerr << "LinePlotter::draw cannot handle dimensionality of the data" << std::endl;
        return;
    }
    if (array.dimensionCount() == 1) {
        draw_1d(array);
    } else {
        draw_2d(array);
    }
}


void LinePlotter::draw_1d(const nix::DataArray &array) {
    nix::Dimension d = array.getDimension(1);
    QVector<double> x_axis, y_axis;
    QVector<QString> x_tick_labels;
    data_array_to_qvector(array, x_axis, y_axis, x_tick_labels, 1);

    QString y_label;
    QVector<QString> ax_labels;
    data_array_ax_labels(array, y_label, ax_labels);

    if (d.dimensionType() == nix::DimensionType::Range) {
        this->add_events(x_axis, y_axis, QString::fromStdString(array.name()), true);
        this->set_ylabel(y_label);
        this->set_xlabel(ax_labels[0]);
        this->set_label(array.name());
    } else {
        this->add_line_plot(x_axis, y_axis, QString::fromStdString(array.name()));
        this->set_ylabel(y_label);
        this->set_xlabel(ax_labels[0]);
        this->set_label(array.name());
    }
}


void LinePlotter::draw_2d(const nix::DataArray &array) {
    int best_dim = guess_best_xdim(array);
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
}


int LinePlotter::guess_best_xdim(const nix::DataArray &array) const {
    if (array.dimensionCount() > 1){
        if (array.getDimension(1).dimensionType() == nix::DimensionType::Sample ||
                array.getDimension(1).dimensionType() == nix::DimensionType::Range) {
            return 1;
        } else {
            nix::DimensionType dt_2 = array.getDimension(2).dimensionType();
            if (dt_2 != nix::DimensionType::Set)
                return 2;
        }
    }
    return 1;
}


bool LinePlotter::check_dimensions(const nix::DataArray &array) const {
    if (array.dimensionCount() == 0) {
        return false;
    }
    if (array.dimensionCount() == 1) {
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
        return true;
    }
    return false;
}


void LinePlotter::add_line_plot(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name) {
    ui->plot->addGraph();
    QPen pen;
    pen.setColor(cmap.next());
    ui->plot->graph()->setPen(pen);
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


void LinePlotter::add_events(const QVector<double> &x_data, const QString &name, bool y_scale) {
    double max = ui->plot->yAxis->range().upper * 0.9;
    QVector<double> y_data(x_data.size(), max);
    add_events(x_data, y_data, name, y_scale);
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
        ui->plot->addItem(rect);
    }
}


QCustomPlot* LinePlotter::get_plot() {
    return ui->plot;
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
            graph->setSelected(true);
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

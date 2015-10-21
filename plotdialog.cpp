#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "common/Common.hpp"
#include <QToolBar>
#include <QMenu>

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog) {
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
    this->setCursor(Qt::WaitCursor);
    if (item.canConvert<nix::DataArray>()) {
        nix::DataArray array = item.value<nix::DataArray>();
        ui->plot->clearGraphs();
        ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
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
    this->setCursor(Qt::ArrowCursor);
}


void PlotDialog::draw_1d(const nix::DataArray &array) {
    if (check_plottable_dtype(array)) {
        nix::Dimension d = array.getDimension(1);
        QVector<double> x_axis, y_axis;
        double x_min, x_max, y_min, y_max;
        std::string x_label, y_label;
        if (d.dimensionType() == nix::DimensionType::Sample) {
            nix::SampledDimension dim = d.asSampledDimension();
            std::vector<double> ax = dim.axis(array.dataExtent()[0]);
            x_axis = QVector<double>::fromStdVector(ax);
            if (dim.label())
                x_label = *dim.label();
            if (dim.unit())
                x_label = x_label + " [" + *dim.unit() + "]";

            std::vector<double> data(ax.size());
            array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
            y_axis = QVector<double>::fromStdVector(data);

            if (array.label())
                y_label = *array.label();
            if (array.unit())
                y_label = y_label + " [" + *array.unit() + "]";
            x_min = x_axis[0];
            x_max = x_axis.back();
            add_line_plot(x_axis, y_axis);
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            nix::RangeDimension dim = d.asRangeDimension();
            std::vector<double> ax = dim.axis(array.dataExtent()[0]);
            x_axis = QVector<double>::fromStdVector(ax);

            if (dim.label())
                x_label = *dim.label();
            if (dim.unit())
                x_label = x_label + " [" + *dim.unit() + "]";
            std::vector<double> data(ax.size());
            array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
            y_axis = QVector<double>::fromStdVector(data);

            if (array.label())
                y_label = *array.label();
            if (array.unit())
                y_label = y_label + " [" + *array.unit() + "]";
            add_scatter_plot(x_axis, y_axis);
            x_min = x_axis[0];
            x_max = x_axis.back();
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            std::vector<double> data;
            array.getData(data);
            y_axis = QVector<double>::fromStdVector(data);

            nix::SetDimension dim = d.asSetDimension();
            std::vector<string> labels = dim.labels();
            QVector<QString> categories;

            if (labels.size() == 0) {
                for (int i = 0; i < y_axis.size(); ++i)
                    categories.push_back(QString::fromStdString(nix::util::numToStr<int>(i)));
            }
            x_min = 0;
            x_max = labels.size();
            add_bar_plot(QVector<string>);
        } else {
            std::cerr << "unsupported dimension type" << std::endl;
        }

        double y_min = *std::min_element(std::begin(y_axis), std::end(y_axis));
        double y_max = *std::max_element(std::begin(y_axis), std::end(y_axis));

        ui->plot->graph()->setName(array.name().c_str());
        ui->plot->xAxis->setRange(x_min, x_max);
        ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
        ui->plot->xAxis->setLabel(QString(x_label.c_str()));
        ui->plot->yAxis->setLabel(QString(y_label.c_str()));
        ui->plot->replot();
    }
}

void PlotDialog::add_line_plot(QVector<double> x_data, QVector<double> y_data) {
    ui->plot->addGraph(x_data, y_data);
}


void PlotDialog::add_scatter_plot(QVector<double> x_data, QVector<double> y_data) {
    ui->plot->addGraph(x_data, y_data);
    QPen pen = ui->plot->graph()->pen();
    pen.setStyle(Qt::PenStyle::NoPen);
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->plot->graph()->setPen(pen);
}


void PlotDialog::add_bar_plot(QVector<QString> categories, QVector<double> y_data){
    return;
}

void PlotDialog::draw_2d(const nix::DataArray &array) {

}


bool PlotDialog::check_plottable_dtype(const nix::DataArray &array) {
    bool plottable = true;
    plottable = plottable && array.dataType() != nix::DataType::DateTime;
    plottable = plottable && array.dataType() != nix::DataType::Bool;
    plottable = plottable && array.dataType() != nix::DataType::String;
    plottable = plottable && array.dataType() != nix::DataType::Date;
    plottable = plottable && array.dataType() != nix::DataType::Char;
    plottable = plottable && array.dataType() != nix::DataType::Opaque;
    plottable = plottable && array.dataType() != nix::DataType::Nothing;
    return plottable;
}


bool PlotDialog::can_draw() {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() | item.canConvert<nix::Tag>();
}


void PlotDialog::show_context_menu() {
    std::cerr << "show_context menu!" << std::endl;
}


void PlotDialog::selection_changed()
{
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


void PlotDialog::mouse_press() {
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeDrag(ui->plot->xAxis->orientation());
    else if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeDrag(ui->plot->yAxis->orientation());
    else
        ui->plot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}


void PlotDialog::mouse_wheel() {
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (ui->plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeZoom(ui->plot->xAxis->orientation());
    else if (ui->plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->plot->axisRect()->setRangeZoom(ui->plot->yAxis->orientation());
    else
        ui->plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}


void PlotDialog::context_menu_request(QPoint pos) {
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


void PlotDialog::remove_selected_graph() {
    if (ui->plot->selectedGraphs().size() > 0) {
        ui->plot->removeGraph(ui->plot->selectedGraphs().first());
        ui->plot->replot();
    }
}


void PlotDialog::show_legend() {
    ui->plot->legend->setVisible(!ui->plot->legend->visible());
    ui->plot->replot();
}


void PlotDialog::clear_selection(){
    ui->plot->deselectAll();
    ui->plot->replot();
}


void PlotDialog::set_marker(QString marker){
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

void PlotDialog::marker_circle() {
    set_marker("circle");
}

void PlotDialog::marker_cross() {
    set_marker("cross");
}

void PlotDialog::marker_dot() {
    set_marker("dot");
}

void PlotDialog::marker_diamond() {
    set_marker("diamond");
}

void PlotDialog::marker_square() {
    set_marker("square");
}

void PlotDialog::marker_none(){
    set_marker("none");
}

void PlotDialog::marker_plus(){
    set_marker("plus");
}

void PlotDialog::set_pen_style(QString style) {
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

void PlotDialog::set_pen_none() {
    set_pen_style("none");
}

void PlotDialog::set_pen_solid() {
    set_pen_style("solid");
}

void PlotDialog::set_pen_dashed() {
    set_pen_style("dashed");
}

void PlotDialog::set_pen_dotted() {
    set_pen_style("dotted");
}

void PlotDialog::set_pen_dashdotted() {
    set_pen_style("dashdot");
}

PlotDialog::~PlotDialog()
{
    delete ui;
}

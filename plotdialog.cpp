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
        ui->plot->addGraph();
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
    nix::Dimension d = array.getDimension(1);
    QVector<double> x_axis, y_axis;
    double x_min = 0.0, x_max=1.0, y_min=-1.0, y_max=1.0;
    std::string x_label, y_label;
    QString name;
    name.fromStdString(array.name());
    if (d.dimensionType() == nix::DimensionType::Sample) {
        nix::SampledDimension dim = d.asSampledDimension();
        std::vector<double> ax = dim.axis(array.dataExtent()[0]);
        x_axis = QVector<double>::fromStdVector(ax);
        x_min = ax[0];
        x_max = ax.back();
        if (dim.label())
            x_label = *dim.label();
        if (dim.unit())
            x_label = x_label + " [" + *dim.unit() + "]";

        std::vector<double> data(ax.size());
        array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
        y_axis = QVector<double>::fromStdVector(data);
        y_min = *std::min_element(std::begin(data), std::end(data));
        y_max = *std::max_element(std::begin(data), std::end(data));
        if (array.label())
            y_label = *array.label();
        if (array.unit())
            y_label = y_label + " [" + *array.unit() + "]";
    } else if (d.dimensionType() == nix::DimensionType::Range) {
        std::cerr << "1D sampled" << std::endl;

    } else {

    }
    ui->plot->graph(0)->setName(array.name().c_str());
    ui->plot->graph(0)->setData(x_axis, y_axis);
    ui->plot->xAxis->setRange(x_min, x_max);
    ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    ui->plot->xAxis->setLabel(QString(x_label.c_str()));
    ui->plot->yAxis->setLabel(QString(y_label.c_str()));
    ui->plot->replot();
}


void PlotDialog::draw_2d(const nix::DataArray &array) {

}


bool PlotDialog::can_draw() {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() | item.canConvert<nix::Tag>();
}


void PlotDialog::show_context_menu() {
    std::cerr << "show_context menu!" << std::endl;
}


void PlotDialog::selection_changed()
{
    /*
    normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
    the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
    and the axis base line together. However, the axis label shall be selectable individually.

    The selection state of the left and right axes shall be synchronized as well as the state of     the
    bottom and top axes

    Further, we want to synchronize the selection of the graphs with the selection state of the respective
    legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
    or on its legend item.
    */

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


PlotDialog::~PlotDialog()
{
    delete ui;
}

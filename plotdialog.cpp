#include "plotdialog.h"
#include "ui_plotdialog.h"
#include "common/Common.hpp"
#include <QToolBar>
#include <QMenu>
#include <QInputDialog>


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
    ui->plot->clearGraphs();
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
    if (item.canConvert<nix::DataArray>()) {
        nix::DataArray array = item.value<nix::DataArray>();
        draw_data_array(array);
    } else if (item.canConvert<nix::Tag>()) {
        nix::Tag tag = item.value<nix::Tag>();
        draw_tag(tag);
    }  else if (item.canConvert<nix::MultiTag>()){
        nix::MultiTag mtag = item.value<nix::MultiTag>();
        draw_multi_tag(mtag);
    } else {
        std::cerr << "Cannot plot the passed entity." << std::endl;
    }
    this->setCursor(Qt::ArrowCursor);
}


void PlotDialog::draw_tag(const nix::Tag &tag) {
    if (tag.referenceCount() == 1) {
        draw_data_array(tag.references()[0]);
        if (tag.position().size() == 1) {
            draw_segment(tag);
        } else {
            std::cerr << "Can only draw regions in one-d, so far!" << std::endl;
        }
    } else {
        std::vector<nix::DataArray> arrays = tag.references();
        QStringList array_names;
        for (size_t i = 0; i < arrays.size(); ++i)
            array_names.append(QString::fromStdString(arrays[i].name()));
        bool ok;
        QString label = "Select one of the referenced data arrays for display:";
        QString item = QInputDialog::getItem(this, "Select data array", label, array_names, 0, false, &ok);
        if (ok && !item.isEmpty())
            draw_data_array(tag.getReference(item.toStdString()));
        if (tag.position().size() == 1) {
            draw_segment(tag);
        } else {
            std::cerr << "Can only draw regions in one-d, so far!" << std::endl;
        }
    }
}

void PlotDialog::draw_segment(const nix::Tag &tag) {
    QCPItemRect *rect = new QCPItemRect(ui->plot);
    double y_max, y_min;
    y_max = ui->plot->yAxis->range().upper;
    y_min = ui->plot->yAxis->range().lower;
    double x_min, x_max;
    x_min = tag.position()[0];
    x_max = x_min + 0.0001; // TODO set this to the sample_interval?!
    if (tag.extent().size() == 1)
       x_max = tag.position()[0] + tag.extent()[0];
    rect->position("topLeft")->setCoords(x_min, y_max);
    rect->position("bottomRight")->setCoords(x_max, y_min);
    rect->setPen(QPen(Qt::red));
    rect->setBrush(QBrush(QColor(255, 10, 10, 50)));
    ui->plot->addItem(rect);
}


void PlotDialog::draw_multi_tag(const nix::MultiTag &mtag) {
    if (mtag.referenceCount() == 1) {
        draw_data_array(mtag.references()[0]);
        if (mtag.positions().dimensionCount() == 1) {
            draw_data_array(mtag.positions());
        } else {
            std::cerr << "Can only draw one-d data, so far!" << std::endl;
        }
    } else {
        std::vector<nix::DataArray> arrays = mtag.references();
        QStringList array_names;
        for (size_t i = 0; i < arrays.size(); ++i)
            array_names.append(QString::fromStdString(arrays[i].name()));
        bool ok;
        QString label = "Select one of the referenced data arrays for display:";
        QString item = QInputDialog::getItem(this, "Select data array", label, array_names, 0, false, &ok);
        if (ok && !item.isEmpty())
            draw_data_array(mtag.getReference(item.toStdString()));
        if (mtag.positions().dimensionCount() == 1) {
        //    draw_segment(mtag);
        } else {
            std::cerr << "Can only draw regions in one-d, so far!" << std::endl;
        }
    }

    std::cerr << "Plotting of multitags is not yet supported." << std::endl;
}


void PlotDialog::draw_data_array(const nix::DataArray &array) {
    size_t dim_count = array.dimensionCount();
    switch (dim_count) {
    case 1:
        draw_1d(array);
        break;
    case 2:
        draw_2d(array);
        break;
    default:
        std::cerr << "Sorry, cannot plot data with more than 2d!" << std::endl;
        break;
    }
}


void PlotDialog::draw_1d(const nix::DataArray &array) {
    if (check_plottable_dtype(array)) {
        nix::Dimension d = array.getDimension(1);
        QVector<double> x_axis, y_axis;
        QVector<QString> x_tick_labels;
        data_array_to_qvector(array, x_axis, y_axis, x_tick_labels, 1);

        QString y_label;
        QVector<QString> ax_labels;
        data_array_ax_labels(array, y_label, ax_labels);
        if (d.dimensionType() == nix::DimensionType::Sample) {
            add_line_plot(x_axis, y_axis, QString::fromStdString(array.name()));
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            add_scatter_plot(x_axis, y_axis, QString::fromStdString(array.name()));
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            add_bar_plot(x_tick_labels, y_axis, QString::fromStdString(array.name()));
        } else {
            std::cerr << "unsupported dimension type" << std::endl;
        }
        ui->plot->xAxis->setLabel(ax_labels[0]);
        ui->plot->yAxis->setLabel(y_label);
        ui->plot->replot();
    }
}


void PlotDialog::add_line_plot(QVector<double> x_data, QVector<double> y_data, QString name) {
    ui->plot->addGraph();
    ui->plot->graph()->addData(x_data, y_data);
    ui->plot->xAxis->setRange(x_data[0], x_data.last());
    double y_min = *std::min_element(std::begin(y_data), std::end(y_data));
    double y_max = *std::max_element(std::begin(y_data), std::end(y_data));
    if (y_min == y_max)
        y_min = 0.0;
    ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    ui->plot->graph()->setName(name);
}


void PlotDialog::add_scatter_plot(QVector<double> x_data, QVector<double> y_data, QString name, bool y_scale) {
    ui->plot->addGraph();
    ui->plot->graph()->addData(x_data, y_data);
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    ui->plot->graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
    ui->plot->graph()->setName(name);

    if (y_scale) {
        double y_min = *std::min_element(std::begin(y_data), std::end(y_data));
        double y_max = *std::max_element(std::begin(y_data), std::end(y_data));
        if (y_min == y_max)
            y_min = 0.0;

        ui->plot->yAxis->setRange(1.05*y_min, 1.05*y_max);
    }
    ui->plot->xAxis->setRange(x_data[0], x_data.last());
}


void PlotDialog::add_bar_plot(QVector<QString> categories, QVector<double> y_data, QString name){
    QCPBars *bars = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);
    ui->plot->addPlottable(bars);

    QPen pen;
    pen.setColor(QColor(150, 222, 0));
    bars->setPen(pen);
    bars->setBrush(QColor(150, 222, 0, 70));
    QVector<double> ticks;
    for (int i = 0; i < categories.size(); ++i)
        ticks.push_back(i);

    ui->plot->xAxis->setAutoTicks(false);
    ui->plot->xAxis->setAutoTickLabels(false);
    ui->plot->xAxis->setTickVector(ticks);
    ui->plot->xAxis->setTickVectorLabels(categories);
    ui->plot->xAxis->setTickLabelRotation(60);
    ui->plot->xAxis->setSubTickCount(0);
    ui->plot->xAxis->setTickLength(0, 4);
    ui->plot->xAxis->grid()->setVisible(true);
    ui->plot->xAxis->setRange(-0.5, categories.size() - 0.5);

    ui->plot->yAxis->setPadding(5); // a bit more space to the left border
    ui->plot->yAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
    ui->plot->yAxis->grid()->setSubGridVisible(true);
    double y_max = *std::max_element(std::begin(y_data), std::end(y_data));
    ui->plot->yAxis->setRange(0, 1.05 * y_max);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    ui->plot->yAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    ui->plot->yAxis->grid()->setSubGridPen(gridPen);

    // Add data:
    bars->setData(ticks, y_data);
    bars->setName(name);
}


void PlotDialog::draw_2d(const nix::DataArray &array) {

}


bool PlotDialog::check_plottable_dtype(const nix::DataArray &array) {
    bool plottable = true;
    plottable = plottable && array.dataType() != nix::DataType::Bool;
    plottable = plottable && array.dataType() != nix::DataType::String;
    plottable = plottable && array.dataType() != nix::DataType::Char;
    plottable = plottable && array.dataType() != nix::DataType::Opaque;
    plottable = plottable && array.dataType() != nix::DataType::Nothing;
    return plottable;
}


bool PlotDialog::can_draw() {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() | item.canConvert<nix::Tag>();
}

void PlotDialog::data_array_ax_labels(const nix::DataArray &array, QString &ylabel, QVector<QString> &labels) {
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


void PlotDialog::data_array_to_qvector(const nix::DataArray &array, QVector<double> &xdata, QVector<double> &ydata, QVector<QString> &xticklabels, nix::ndsize_t dim_index) {
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

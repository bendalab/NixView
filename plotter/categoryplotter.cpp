#include "categoryplotter.h"
#include "ui_categoryplotter.h"

CategoryPlotter::CategoryPlotter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CategoryPlotter),
    cmap()
{
    ui->setupUi(this);
}

CategoryPlotter::~CategoryPlotter()
{
    delete ui;
}


void CategoryPlotter::draw(const nix::DataArray &array) {
    if (array.dimensionCount() > 2) {
        std::cerr << "CategoryPlotter::draw cannot draw 3D!" << std::endl;
        return;
    }
    if (!Plotter::check_plottable_dtype(array)) {
        std::cerr << "CategoryPlotter::draw cannot handle data type " << array.dataType() << std::endl;
        return;
    }
    if (!check_dimensions(array)) {
        std::cerr << "CategroyPlotter::draw cannot handle dimensionality of the data" << std::endl;
        return;
    }
    if (array.dimensionCount() == 1) {
        draw_1d(array);
    } else {
        draw_2d(array);
    }
}


void CategoryPlotter::draw_1d(const nix::DataArray &array) {
    nix::Dimension d = array.getDimension(1);
    QVector<double> x_axis, y_axis;
    QVector<QString> x_tick_labels;
    data_array_to_qvector(array, x_axis, y_axis, x_tick_labels, 1);

    QString y_label;
    QVector<QString> ax_labels;
    Plotter::data_array_ax_labels(array, y_label, ax_labels);
    add_bar_plot(x_tick_labels, y_axis, QString::fromStdString(array.name()));
    set_label(array.name());
    set_ylabel((array.label() ? *array.label() : "") + (array.unit() ? (" [" + *array.unit() + "]") : ""));
}


void CategoryPlotter::draw_2d(const nix::DataArray &array) {
    int best_dim = guess_best_xdim(array);
    QVector<double> xaxis, yaxis;
    QVector<QString> xlabels, ylabels;
    get_data_array_axis(array, xaxis, xlabels, best_dim);
    get_data_array_axis(array, yaxis, ylabels, 3-best_dim);
    double ymin = 0.0;
    double ymax = 0.0;
    QCPBarsGroup *group = new QCPBarsGroup(ui->plot);
    for (int i = 0; i < yaxis.size(); i++) {
        QVector<double> data = get_data_line(array, i, best_dim);
        QCPBars *bars = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);
        bars->setData(xaxis, data);
        QColor c = cmap.next();
        bars->setPen(c);
        c.setAlpha(50);
        bars->setBrush(c);
        bars->setWidth(0.15);
        bars->setBarsGroup(group);
        bars->setName(ylabels[i]);
        double mi = *std::min_element(std::begin(data), std::end(data));
        double ma = *std::max_element(std::begin(data), std::end(data));
        if (mi < ymin)
            ymin = mi;
        if (ma > ymax)
            ymax= ma;
    }


    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->setSubTickCount(0);
    textTicker->addTicks(xaxis,xlabels);
    ui->plot->xAxis->setTicker(textTicker);

    ui->plot->xAxis->grid()->setVisible(true);
    ui->plot->legend->setVisible(true);

    ui->plot->xAxis->setRange(-0.5, xaxis.size());
    ui->plot->yAxis->setRange(1.5 * ymin, 1.5 * ymax);
    ui->plot->xAxis->setTickLength(0, 4);
    ui->plot->xAxis->setTickLabelRotation(60);

    set_label(array.name());
    nix::Dimension dim = array.getDimension(3-best_dim);
    std::string unit = "";
    if (array.label())
        set_ylabel(*array.label() + (array.unit() ? (" [" + *array.unit() + "]") : ""));
}


bool CategoryPlotter::check_dimensions(const nix::DataArray &array) const {
    if (array.dimensionCount() == 0) {
        return false;
    }
    if (array.dimensionCount() == 1) {
        return true;
    }
    nix::DimensionType dt_1 = array.getDimension(1).dimensionType();
    nix::DimensionType dt_2 = array.getDimension(2).dimensionType();
    if (((dt_1 == nix::DimensionType::Sample || dt_1 == nix::DimensionType::Range) && dt_2 == nix::DimensionType::Set) ||
            (dt_1 == nix::DimensionType::Set && (dt_2 == nix::DimensionType::Range || dt_2 == nix::DimensionType::Range))) {
        std::cerr << "CategoryPlotter should draw 2D data with Range or SampleDimension? You serious? I can do this, but seems suboptimal..." << std::endl;
        return true;
    }
    if (dt_1 == nix::DimensionType::Set && dt_2 == nix::DimensionType::Set) {
        return true;
    }
    return false;
}


int CategoryPlotter::guess_best_xdim(const nix::DataArray &array) const {
    nix::NDSize shape = array.dataExtent();
    if (shape[0] > shape[1])
        return 1;
    return 2;
}


PlotterType CategoryPlotter::plotter_type() const {
    return PlotterType::Category;
}


void CategoryPlotter::set_label(const std::string &label) {
    ui->label->setText(QString::fromStdString(label));
}


void CategoryPlotter::set_xlabel(const QString &label){
    ui->plot->xAxis->setLabel(label);
    ui->plot->replot();
}

void CategoryPlotter::set_xlabel(const std::string &label) {
    set_xlabel(QString::fromStdString(label));
}

void CategoryPlotter::set_ylabel(const QString &label){
    ui->plot->yAxis->setLabel(label);
    ui->plot->replot();
}

void CategoryPlotter::set_ylabel(const std::string &label) {
    set_ylabel(QString::fromStdString(label));
}

void CategoryPlotter::add_bar_plot(QVector<QString> categories, QVector<double> y_data, const QString &name){
    QCPBars *bars = new QCPBars(ui->plot->xAxis, ui->plot->yAxis);

    QPen pen;
    pen.setColor(QColor(150, 222, 0));
    bars->setPen(pen);
    bars->setBrush(QColor(150, 222, 0, 70));
    QVector<double> ticks;
    for (int i = 0; i < categories.size(); ++i)
        ticks.push_back(i);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->setSubTickCount(0);
    textTicker->addTicks(ticks,categories);
    ui->plot->xAxis->setTicker(textTicker);

    ui->plot->xAxis->grid()->setVisible(true);
    ui->plot->legend->setVisible(true);

    ui->plot->xAxis->setTickLabelRotation(60);
    ui->plot->xAxis->setTickLength(0, 4);
    ui->plot->xAxis->grid()->setVisible(true);
    ui->plot->xAxis->setRange(-0.5, categories.size() - 0.5);

    ui->plot->yAxis->setPadding(5);
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



void CategoryPlotter::add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) {
    /*
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
    */
}


void CategoryPlotter::add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) {
    /*
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
    */
}


QCustomPlot* CategoryPlotter::get_plot() {
    return ui->plot;
}

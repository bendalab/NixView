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
}

void PlotDialog::set_entity(QVariant var) {
    ui->plot->setEntity(var);
}

/*
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
*/

/*
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
*/

/*
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
*/

/*
void PlotDialog::draw_multi_tag(const nix::MultiTag &mtag) {
    nix::DataArray reference;
    if (mtag.referenceCount() == 1) {
        reference = mtag.getReference(0);
    } else {
        std::vector<nix::DataArray> arrays = mtag.references();
        QStringList array_names;
        for (size_t i = 0; i < arrays.size(); ++i)
            array_names.append(QString::fromStdString(arrays[i].name()));
        bool ok;
        QString label = "Select one of the referenced data arrays for display:";
        QString item = QInputDialog::getItem(this, "Select data array", label, array_names, 0, false, &ok);
        if (ok && !item.isEmpty())
            reference = mtag.getReference(item.toStdString());
    }
    if (mtag.positions().dimensionCount() > 1) {
        std::cerr << "Can only draw one-d data, so far!" << std::endl;
    }
    draw_data_array(reference);
    if (mtag.positions().dimensionCount() > 1) {
        std::cerr << "Can only draw regions in one-d, so far!" << std::endl;
    }
    std::vector<double> pos(mtag.positions().dataExtent()[0]);
    std::vector<double> ext;

    mtag.positions().getData(nix::DataType::Double, pos.data(), {mtag.positions().dataExtent()[0]}, {0});
    QVector<double> positions = QVector<double>::fromStdVector(pos);

    if (mtag.extents() != nix::none) {
        ext.resize(positions.size());
        mtag.extents().getData(nix::DataType::Double, ext.data(), {mtag.positions().dataExtent()[0]}, {0});
    }
    QVector<double> extents = QVector<double>::fromStdVector(ext);
    QString name = QString::fromStdString(mtag.name());

    if (extents.size() > 0) {
        add_segments(positions, extents, name);
    } else {
        QVector<double> y_pos(pos.size());
        y_pos.fill(0.0);
        add_scatter_plot(positions, y_pos, QString::fromStdString(mtag.name()));
    }
}
*/

/*
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
*/

/*
void PlotDialog::add_segments(QVector<double> &positions, QVector<double> &extents, QString &name) {
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
}
*/

PlotDialog::~PlotDialog()
{
    delete ui;
}

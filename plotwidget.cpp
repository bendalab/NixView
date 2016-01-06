#include "plotwidget.h"
#include "ui_plotwidget.h"
#include "plotter/lineplotter.h"
#include "plotter/categoryplotter.h"
#include "common/Common.hpp"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    std::cerr << "PlotWidget" << std::endl;
}

PlotWidget::~PlotWidget()
{
    delete ui;
}


bool PlotWidget::can_draw() const {
    return this->item.canConvert<nix::DataArray>() | this->item.canConvert<nix::MultiTag>() | this->item.canConvert<nix::Tag>();
}


void PlotWidget::process_item() {
    if (this->item.canConvert<nix::DataArray>()) {
        nix::DataArray array = item.value<nix::DataArray>();
        process(array);
    } else if (this->item.canConvert<nix::Tag>()) {
        nix::Tag tag = item.value<nix::Tag>();
        process(tag);
    } else {
        nix::MultiTag mtag = item.value<nix::MultiTag>();
        process(mtag);
    }
}


void PlotWidget::process(const nix::DataArray &array) {
    size_t dim_count = array.dimensionCount();
    switch (dim_count) {
    case 1:
        if (array.getDimension(1).dimensionType() == nix::DimensionType::Sample ||
                array.getDimension(1).dimensionType() == nix::DimensionType::Range) {
            // delete_widgets_from_layout();
            LinePlotter *lp = new LinePlotter();
            ui->scrollAreaWidgetContents->layout()->addWidget(lp);
            this->plots.push_back(static_cast<Plotter*>(lp));
            draw_1d(array);
        } else if (array.getDimension(1).dimensionType() == nix::DimensionType::Set) {
            CategoryPlotter *cp = new CategoryPlotter();
            ui->scrollAreaWidgetContents->layout()->addWidget(cp);
            this->plots.push_back(static_cast<Plotter*>(cp));
            draw_1d(array);
        }
        break;
    case 2:
//        draw_2d(array);
        break;
    default:
        std::cerr << "Sorry, cannot plot data with more than 2d!" << std::endl;
        break;
    }
}


void PlotWidget::draw_1d(const nix::DataArray &array) {
    if (check_plottable_dtype(array.dataType())) {
        std::cerr << "ping" << std::endl;
        nix::Dimension d = array.getDimension(1);
        QVector<double> x_axis, y_axis;
        QVector<QString> x_tick_labels;
        Plotter::data_array_to_qvector(array, x_axis, y_axis, x_tick_labels, 1);

        QString y_label;
        QVector<QString> ax_labels;
        Plotter::data_array_ax_labels(array, y_label, ax_labels);

        if (d.dimensionType() == nix::DimensionType::Sample) {
            if (this->plots[this->plots.size() - 1]->plotter_type() != PlotterType::Line) {
              return;
            }
            LinePlotter *plotter = static_cast<LinePlotter*>(this->plots[this->plots.size() - 1]);

            plotter->add_line_plot(x_axis, y_axis, QString::fromStdString(array.name()));
            plotter->set_ylabel(y_label);
            plotter->set_xlabel(ax_labels[0]);
            plotter->set_label(array.name());
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            if (this->plots[this->plots.size() - 1]->plotter_type() != PlotterType::Line) {
              return;
            }

            LinePlotter *plotter = static_cast<LinePlotter*>(this->plots[this->plots.size() - 1]);
            plotter->add_events(x_axis, y_axis, QString::fromStdString(array.name()), true);
            plotter->set_ylabel(y_label);
            plotter->set_xlabel(ax_labels[0]);
            plotter->set_label(array.name());
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            if (this->plots[this->plots.size() - 1]->plotter_type() != PlotterType::Category) {
              return;
            }
            CategoryPlotter *plotter = static_cast<CategoryPlotter*>(this->plots[this->plots.size() - 1]);
            plotter->add_bar_plot(x_tick_labels, y_axis, QString::fromStdString(array.name()));
            plotter->set_label(array.name());
            plotter->set_ylabel(array.label() ? *array.label() : "");
        } else {
            std::cerr << "unsupported dimension type" << std::endl;
        }
    }
}

void PlotWidget::process(const nix::Tag &tag) {
    std::cerr << "process Tag" << std::endl;
    for (nix::ndsize_t i = 0; i < tag.referenceCount(); i++) {
        process(tag.getReference(i));
        Plotter *currplot = this->plots[i];
        QVector<double> positions, extents;
        positions.push_back(tag.position()[0]);
        if (tag.extent().size() > 0)
            extents.push_back(tag.extent()[0]);

        if (currplot->plotter_type() == PlotterType::Category) {
            CategoryPlotter* plt = static_cast<CategoryPlotter*>(this->plots[i]);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, QString::fromStdString(tag.name()));
        } else if (currplot->plotter_type() == PlotterType::Line) {
            LinePlotter *plt = static_cast<LinePlotter*>(this->plots[i]);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, QString::fromStdString(tag.name()));
        }
    }
}

void PlotWidget::process(const nix::MultiTag &mtag) {
    std::cerr << "process MultiTag" << std::endl;

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

    for (nix::ndsize_t i = 0; i < mtag.referenceCount(); i++){
        process(mtag.getReference(i));
        Plotter *currplot = this->plots[i];

        if (currplot->plotter_type() == PlotterType::Category) {
            CategoryPlotter* plt = static_cast<CategoryPlotter*>(this->plots[i]);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, name);
        } else if (currplot->plotter_type() == PlotterType::Line) {
            LinePlotter *plt = static_cast<LinePlotter*>(this->plots[i]);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, name);
        }
    }
    /*
    if (extents.size() > 0) {
        add_segments(positions, extents, name);
    } else {
        QVector<double> y_pos(pos.size());
        y_pos.fill(0.0);
        add_scatter_plot(positions, y_pos, QString::fromStdString(mtag.name()));
    }
    */
}

void PlotWidget::setEntity(QVariant var) {
    this->item = var;
    process_item();
    if (can_draw()) {
        draw();
    }
}


void PlotWidget::draw() {

}


bool PlotWidget::check_plottable_dtype(nix::DataType dtype) const {
    bool plottable = true;
    plottable = plottable && dtype != nix::DataType::Bool;
    plottable = plottable && dtype != nix::DataType::String;
    plottable = plottable && dtype != nix::DataType::Char;
    plottable = plottable && dtype != nix::DataType::Opaque;
    plottable = plottable && dtype != nix::DataType::Nothing;
    return plottable;
}

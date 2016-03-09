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
    item = nullptr;
}

PlotWidget::~PlotWidget()
{
    delete ui;
}


bool PlotWidget::can_draw() const {
    return (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0) |
           (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0) |
           (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0);
}


void PlotWidget::process_item() {
    if (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0)
    {
        nix::DataArray array = item->get_nix_entity<nix::DataArray>();
        process(array);
        describe(array);
    }
    else if (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0)
    {
        nix::Tag tag = item->get_nix_entity<nix::Tag>();
        process(tag);
        describe(tag);
    }
    else if (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0)
    {
        nix::MultiTag mtag = item->get_nix_entity<nix::MultiTag>();
        process(mtag);
        describe(mtag);
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
        if (array.getDimension(1).dimensionType() == nix::DimensionType::Sample ||
                array.getDimension(1).dimensionType() == nix::DimensionType::Range) {
            ui->multiPlotCheckBox->setEnabled(true);
            if (array.getDimension(2).dimensionType() == nix::DimensionType::Set) {
                draw_multi_line(array);
            } else {
                // handle 2D image/heatmap plotting not supported, yet TODO
            }
        } else {
            // handle 2 D set plotting  TODO
        }
        break;
    default:
        std::cerr << "Sorry, cannot plot data with more than 2d!" << std::endl;
        break;
    }
}


void PlotWidget::draw_multi_line(const nix::DataArray &array) {
    if (ui->multiPlotCheckBox->isChecked()) {
        // TODO
    } else {
        LinePlotter *lp = new LinePlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(lp);
        this->plots.push_back(static_cast<Plotter*>(lp));

        QVector<double> x_axis, y_axis;
        QVector<QString> y_tick_labels;
        std::vector<std::string> labels;
        nix::Dimension d = array.getDimension(2);
        if (d.dimensionType() != nix::DimensionType::Set) {
            return;
        }
        nix::SetDimension sd = d.asSetDimension();
        sd.labels(labels);
        lp->get_data_array_axis(array, x_axis, y_tick_labels, 1);
        y_tick_labels.resize(0);
        for (std::string s : labels)
            y_tick_labels.push_back(QString::fromStdString(s));
        for (nix::ndsize_t i = 0; i < array.dataExtent()[1]; i++) {
            std::vector<double> data;
            data.resize(array.dataExtent()[0]);
            nix::NDSize count(array.dataExtent()[0], 1);
            nix::NDSize offset(0, i);
            array.getData(nix::DataType::Double, data.data(), count, offset);
            y_axis = QVector<double>::fromStdVector(data);
            lp->add_line_plot(x_axis, y_axis, y_tick_labels[i]);
        }

    }
}


void PlotWidget::draw_1d(const nix::DataArray &array) {
    if (check_plottable_dtype(array.dataType())) {
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
}

EntityDescriptor PlotWidget::basic_description(const std::string &name, const std::string &type, const std::string &description,
                                   const std::string &id, const std::string &created, const std::string &updated ) {
    EntityDescriptor descr(name, type, description, id, created, updated);
    return descr;
}

void PlotWidget::describe(const nix::DataArray &array) {
    ui->entityDescription->clear();
    EntityDescriptor descr = basic_description(array.name(), array.type(), array.definition() ? *array.definition() : "", array.id(),
                                               nix::util::timeToStr(array.createdAt()), nix::util::timeToStr(array.updatedAt()));
    ui->entityDescription->setText(QString::fromStdString(descr.toHtml()));
}

void PlotWidget::describe(const nix::Tag &tag) {
    ui->entityDescription->clear();
    EntityDescriptor descr = basic_description(tag.name(), tag.type(), tag.definition() ? *tag.definition() : "", tag.id(),
                                               nix::util::timeToStr(tag.createdAt()), nix::util::timeToStr(tag.updatedAt()));
    descr.addSection("References");
    std::vector<nix::DataArray> refs = tag.references();
    for (nix::DataArray a : refs) {
        descr.addItem(a.name());
    }
    ui->entityDescription->setText(QString::fromStdString(descr.toHtml()));
}

void PlotWidget::describe(const nix::MultiTag &mtag) {
    ui->entityDescription->clear();
    EntityDescriptor descr = basic_description(mtag.name(), mtag.type(), mtag.definition() ? *mtag.definition() : "", mtag.id(),
                                               nix::util::timeToStr(mtag.createdAt()), nix::util::timeToStr(mtag.updatedAt()));
    descr.addSection("References");
    std::vector<nix::DataArray> refs = mtag.references();
    for (nix::DataArray a : refs) {
        descr.addItem(a.name());
    }
    ui->entityDescription->setText(QString::fromStdString(descr.toHtml()));
}

void PlotWidget::setEntity(QModelIndex qml) {
    this->item_qml = qml;
    this->item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (can_draw()) {
        process_item();
    }
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

#include "plotwidget.h"
#include "ui_plotwidget.h"
#include "plotter/lineplotter.h"
#include "plotter/categoryplotter.h"
#include "plotter/imageplotter.h"
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


void PlotWidget::delete_widgets_from_layout() {
    if (!ui->scrollAreaWidgetContents->layout()->isEmpty()) {
        QLayoutItem *item = ui->scrollAreaWidgetContents->layout()->itemAt(0);
        if (item->widget()) {
            ui->scrollAreaWidgetContents->layout()->removeItem(item);
            delete item;
        }
    }
}


Plotter* PlotWidget::process(const nix::DataArray &array) {
    PlotterType suggestion = Plotter::suggested_plotter(array);

    if (suggestion == PlotterType::Line) {
        delete_widgets_from_layout();
        LinePlotter *lp = new LinePlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(lp);
        lp->draw(array);
        plot = lp;
    } else if (suggestion == PlotterType::Category) {
        delete_widgets_from_layout();
        CategoryPlotter *cp = new CategoryPlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(cp);
        cp->draw(array);
        plot = cp;
    } else if (suggestion == PlotterType::Image) {
        delete_widgets_from_layout();
        ImagePlotter *ip = new ImagePlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(ip);
        ip->draw(array);
        plot = ip;
    }

    return plot;
}


void PlotWidget::process(const nix::Tag &tag) {
    for (nix::ndsize_t i = 0; i < tag.referenceCount(); i++) {
        Plotter *currplot = process(tag.getReference(i));
        QVector<double> positions, extents;
        positions.push_back(tag.position()[0]);
        if (tag.extent().size() > 0)
            extents.push_back(tag.extent()[0]);
        if (currplot != nullptr && currplot->plotter_type() == PlotterType::Category) {
            CategoryPlotter* plt = static_cast<CategoryPlotter*>(currplot);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, QString::fromStdString(tag.name()));
        } else if (currplot != nullptr && currplot->plotter_type() == PlotterType::Line) {
            LinePlotter *plt = static_cast<LinePlotter*>(currplot);
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
        Plotter *currplot = process(mtag.getReference(i));
        if (currplot != nullptr && currplot->plotter_type() == PlotterType::Category) {
            CategoryPlotter* plt = static_cast<CategoryPlotter*>(currplot);
            plt->setFixedHeight(200);
            plt->add_segments(positions, extents, name);
        } else if (currplot != nullptr && currplot->plotter_type() == PlotterType::Line) {
            LinePlotter *plt = static_cast<LinePlotter*>(currplot);
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

void PlotWidget::savePlot() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), "untitled.pdf", tr("Images (*.pdf)"));

    if (fileName.isEmpty()) {
        return;
    }

    QCustomPlot *qcp = plot->get_plot();
    qcp->savePdf(fileName);
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

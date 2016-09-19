#include "plotwidget.h"
#include "ui_plotwidget.h"
#include "lineplotter.h"
#include "categoryplotter.h"
#include "imageplotter.h"
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
    std::cerr << "PlotWidget destructor!" << std::endl;
    delete ui;
}


bool PlotWidget::can_draw() const {
    NixType type = item->nixType();
    return (type == NixType::NIX_DATA_ARRAY) | (type == NixType::NIX_MTAG)
            | (type == NixType::NIX_TAG) | (type == NixType::NIX_FEAT);
}


void PlotWidget::process_item() {
    if (this->item->nixType() == NixType::NIX_DATA_ARRAY) {
        nix::DataArray array = item->itemData().value<nix::DataArray>();
        process(array);
    } else if (item->nixType() == NixType::NIX_TAG) {
        nix::Tag tag = item->itemData().value<nix::Tag>();
        process(tag);
    } else if (item->nixType() == NixType::NIX_MTAG) {
        nix::MultiTag mtag = item->itemData().value<nix::MultiTag>();
        process(mtag);
    } else if (item->nixType() == NixType::NIX_FEAT) {
        nix::Feature feat = item->itemData().value<nix::Feature>();
        process(feat.data());
    }
}


void PlotWidget::delete_widgets_from_layout() {
    if ((ui->scrollAreaWidgetContents->layout() != NULL) && (!ui->scrollAreaWidgetContents->layout()->isEmpty())) {
        QLayoutItem *item = ui->scrollAreaWidgetContents->layout()->itemAt(0);
        if (item->widget()) {
            ui->scrollAreaWidgetContents->layout()->removeItem(item);
            delete item;
        }
    }
}


Plotter* PlotWidget::process(const nix::DataArray &array) {
    this->text = QString::fromStdString(EntityDescriptor::describe(array));
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


void PlotWidget::process(const nix::Tag &tag, nix::ndsize_t ref) {
    if (tag.referenceCount() == 0) {
        return;
    }
    this->text = QString::fromStdString(EntityDescriptor::describe(tag));

    if (ref >= tag.referenceCount()) {
        ref = tag.referenceCount() -1;
    }

    Plotter *currplot = process(tag.getReference(ref));
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


void PlotWidget::process(const nix::Feature & feat, const nix::Tag & tag) {
    nix::DataArray da = feat.data();
    Plotter *currplot = process(da);
    this->text = QString::fromStdString(EntityDescriptor::describe(feat));
    QVector<double> positions, extents;
    positions.push_back(tag.position()[0]);
    if (tag.extent().size() > 0)
        extents.push_back(tag.extent()[0]);
    if (currplot != nullptr && currplot->plotter_type() == PlotterType::Category) {
        CategoryPlotter* plt = static_cast<CategoryPlotter*>(currplot);
        plt->setFixedHeight(200);
        if (feat.linkType() == nix::LinkType::Tagged) {
            plt->add_segments(positions, extents, QString::fromStdString(tag.name()));
        }
    } else if (currplot != nullptr && currplot->plotter_type() == PlotterType::Line) {
        LinePlotter *plt = static_cast<LinePlotter*>(currplot);
        plt->setFixedHeight(200);
        if (feat.linkType() == nix::LinkType::Tagged) {
            plt->add_segments(positions, extents, QString::fromStdString(tag.name()));
        }
    }
}


void PlotWidget::process(const nix::MultiTag &mtag, nix::ndsize_t ref) {
    if (mtag.referenceCount() == 0)
        return;
    this->text = QString::fromStdString(EntityDescriptor::describe(mtag));
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
    if (ref >= mtag.referenceCount()) {
        ref = mtag.referenceCount() -1;
    }
    Plotter *currplot = process(mtag.getReference(ref));
    if (currplot != nullptr && currplot->plotter_type() == PlotterType::Category) {
        CategoryPlotter* plt = static_cast<CategoryPlotter*>(currplot);
        plt->setFixedHeight(200);
        plt->add_segments(positions, extents, name);
    } else if (currplot != nullptr && currplot->plotter_type() == PlotterType::Line) {
        LinePlotter *plt = static_cast<LinePlotter*>(currplot);
        plt->setFixedHeight(200);
        if (extents.size() > 0) {
            plt->add_segments(positions, extents, name);
        } else {
            plt->add_events(positions, name, false);
        }
    }
}


void PlotWidget::setEntity(QModelIndex qml) {
    this->item_qml = qml;
    this->item = static_cast<NixTreeModelItem*>(qml.internalPointer());
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


void PlotWidget::clear() {
    delete_widgets_from_layout();
    this->text = "";
    //this->repaint();
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


void PlotWidget::show_more() {
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    if (this->text.size() < 1) {
        msgBox.setText("no description available!");
    } else {
        msgBox.setText(this->text);
    }
    msgBox.exec();
}

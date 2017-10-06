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

    connect(this->ui->hScrollBar, SIGNAL(valueChanged(int)), this, SLOT(hScrollBarPosChanged(int)));
    connect(this->ui->vScrollBar, SIGNAL(valueChanged(int)), this, SLOT(vScrollBarPosChanged(int)));

    ui->hScrollBar->setRange(1,1);
    ui->vScrollBar->setRange(1,1);
    ui->hScrollBar->setEnabled(false);
    ui->vScrollBar->setEnabled(false);
    scrollFaktor = 100;

}

PlotWidget::~PlotWidget()
{
    delete ui;
}


bool PlotWidget::can_draw() const {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() |
            item.canConvert<nix::Tag>() | item.canConvert<nix::Feature>();
}


void PlotWidget::process_item() {
    if (item.canConvert<nix::DataArray>()) {
        nix::DataArray array = item.value<nix::DataArray>();
        process(array);
    } else if (item.canConvert<nix::Tag>()) {
        nix::Tag tag = item.value<nix::Tag>();
        process(tag);
    } else if (item.canConvert<nix::MultiTag>()) {
        nix::MultiTag mtag = item.value<nix::MultiTag>();
        process(mtag);
    } else if (item.canConvert<nix::Feature>()) {
        nix::Feature feat = item.value<nix::Feature>();
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

        ui->hScrollBar->setEnabled(true);
        ui->vScrollBar->setEnabled(true);

        connect(lp,   SIGNAL(xAxisChanged(QCPRange, QCPRange)),        this, SLOT(changeHScrollBarValue(QCPRange, QCPRange)) );
        connect(lp,   SIGNAL(yAxisChanged(QCPRange, QCPRange)),        this, SLOT(changeVScrollBarValue(QCPRange, QCPRange)) );
        connect(this, SIGNAL(hScrollValueChanged(double)), lp,   SLOT(changeXAxisRange(double)) );
        connect(this, SIGNAL(vScrollValueChanged(double)), lp,   SLOT(changeYAxisRange(double)) );

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


void PlotWidget::process(const nix::Feature & feat, const nix::MultiTag & mtag) {
    nix::DataArray da = feat.data();
    Plotter *currplot = process(da);
    this->text = QString::fromStdString(EntityDescriptor::describe(feat));
    std::vector<double> pos(mtag.positions().dataExtent()[0]);
    std::vector<double> ext;

    mtag.positions().getData(nix::DataType::Double, pos.data(), {mtag.positions().dataExtent()[0]}, {0});
    QVector<double> positions = QVector<double>::fromStdVector(pos);

    if (mtag.extents() != nix::none) {
        ext.resize(positions.size());
        mtag.extents().getData(nix::DataType::Double, ext.data(), {mtag.positions().dataExtent()[0]}, {0});
    }
    QVector<double> extents = QVector<double>::fromStdVector(ext);

    if (currplot != nullptr && currplot->plotter_type() == PlotterType::Category) {
        CategoryPlotter* plt = static_cast<CategoryPlotter*>(currplot);
        plt->setFixedHeight(200);
        if (feat.linkType() == nix::LinkType::Tagged) {
            plt->add_segments(positions, extents, QString::fromStdString(mtag.name()));
        }
    } else if (currplot != nullptr && currplot->plotter_type() == PlotterType::Line) {
        LinePlotter *plt = static_cast<LinePlotter*>(currplot);
        plt->setFixedHeight(200);
        if (feat.linkType() == nix::LinkType::Tagged) {
            plt->add_segments(positions, extents, QString::fromStdString(mtag.name()));
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


void PlotWidget::setEntity(QVariant var) {
    this->item = var;
    if (can_draw()) {
        process_item();
    }
}

void PlotWidget::save_plot() {
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

void PlotWidget::vScrollBarPosChanged(int value) {
    value = std::abs(value);
    double centerRatio = (double) value / (double) std::abs(ui->vScrollBar->minimum());
    emit vScrollValueChanged(centerRatio);
}

void PlotWidget::hScrollBarPosChanged(int value) {
    value = std::abs(value);
    double centerRatio = (double) value / (double) std::abs(ui->hScrollBar->minimum());
    emit hScrollValueChanged(centerRatio);
}

void PlotWidget::changeHScrollBarValue(QCPRange newRange, QCPRange completeRange) {
    //Umrechnung von QCPRange to int und verschieben der ScrollBar!

        QCPRange hScrollRange = QCPRange( (-1* scrollFaktor * completeRange.size() / newRange.size()) , 0);
        ui->hScrollBar->setRange(hScrollRange.lower, hScrollRange.upper);
        ui->hScrollBar->setPageStep(hScrollRange.size() / ((completeRange.size() / newRange.size())));
        int newPos = -1*(newRange.center()- completeRange.lower) / completeRange.size() * hScrollRange.size();
        ui->hScrollBar->setValue(newPos);
}

void PlotWidget::changeVScrollBarValue(QCPRange newRange, QCPRange completeRange) {
    //Umrechnung von QCPRange to int und verschieben der ScrollBar!

        QCPRange vScrollRange = QCPRange( (-1* scrollFaktor * completeRange.size() / newRange.size()) , 0);
        ui->vScrollBar->setRange(vScrollRange.lower, vScrollRange.upper);
        ui->vScrollBar->setPageStep(vScrollRange.size() / ((completeRange.size() / newRange.size())));
        int newPos = -1*(newRange.center()- completeRange.lower) / completeRange.size() * vScrollRange.size();
        ui->vScrollBar->setValue(newPos);

}












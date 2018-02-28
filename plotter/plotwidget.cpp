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
    connect(this->ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderPosChanged(int)));

    ui->hScrollBar->setRange(1,1);
    ui->vScrollBar->setRange(1,1);
    ui->hScrollBar->setHidden(true);
    ui->vScrollBar->setHidden(true);
    scrollFaktor = 100;
    ui->zoomSlider->setHidden(true);
    ui->zoomLabel->setHidden(true);
    ui->zoomSlider->setRange(-100, -1); // size used as zoomfaktor!

    ui->resetViewButton->setHidden(true);
    zoomMax = 1.1;

}

PlotWidget::~PlotWidget()
{
    delete ui;
}


bool PlotWidget::canDraw() const {
    return item.canConvert<nix::DataArray>() | item.canConvert<nix::MultiTag>() |
            item.canConvert<nix::Tag>() | item.canConvert<nix::Feature>();
}


void PlotWidget::processItem() {
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


void PlotWidget::deleteWidgetsFromLayout() {
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
        deleteWidgetsFromLayout();
        LinePlotter *lp = new LinePlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(lp);

        ui->hScrollBar->setHidden(false);
        ui->vScrollBar->setHidden(false);
        ui->resetViewButton->setHidden(false);
        ui->zoomSlider->setHidden(false);
        ui->zoomLabel->setHidden(false);

        connect(this, SIGNAL(resetViewToPlot()), lp, SLOT(resetView()));

        connect(lp, SIGNAL(xAxisChanged(QCPRange,QCPRange)), this, SLOT(changeSliderPos(QCPRange,QCPRange)));
        connect(this, SIGNAL(sliderToPlot(double)), lp, SLOT(changeXAxisSize(double)));

        connect(lp,   SIGNAL(xAxisChanged(QCPRange, QCPRange)),        this, SLOT(changeHScrollBarValue(QCPRange, QCPRange)) );
        connect(lp,   SIGNAL(yAxisChanged(QCPRange, QCPRange)),        this, SLOT(changeVScrollBarValue(QCPRange, QCPRange)) );
        connect(this, SIGNAL(hScrollBarToPlot(double)), lp,   SLOT(changeXAxisPosition(double)) );
        connect(this, SIGNAL(vScrollBarToPlot(double)), lp,   SLOT(changeYAxisPosition(double)) );


        lp->draw(array);
        plot = lp;

    } else if (suggestion == PlotterType::Category) {
        deleteWidgetsFromLayout();
        CategoryPlotter *cp = new CategoryPlotter();
        ui->scrollAreaWidgetContents->layout()->addWidget(cp);
        cp->draw(array);
        plot = cp;
    } else if (suggestion == PlotterType::Image) {
        deleteWidgetsFromLayout();
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
    if (canDraw()) {
        processItem();
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
    deleteWidgetsFromLayout();
    this->text = "";
    //this->repaint();
}


bool PlotWidget::checkPlottableDType(nix::DataType dtype) const {
    bool plottable = true;
    plottable = plottable && dtype != nix::DataType::Bool;
    plottable = plottable && dtype != nix::DataType::String;
    plottable = plottable && dtype != nix::DataType::Char;
    plottable = plottable && dtype != nix::DataType::Opaque;
    plottable = plottable && dtype != nix::DataType::Nothing;
    return plottable;
}


void PlotWidget::showMore() {
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    if (this->text.size() < 1) {
        msgBox.setText("no description available!");
    } else {
        msgBox.setText(this->text);
    }
    msgBox.exec();
}

void PlotWidget::resetView() {
    emit resetViewToPlot();
}

void PlotWidget::hScrollBarPosChanged(int value) {
    emit hScrollBarToPlot(-value / scrollFaktor);
}

void PlotWidget::vScrollBarPosChanged(int value) {
    emit vScrollBarToPlot(-value / scrollFaktor);
}

void PlotWidget::sliderPosChanged(int value) {
    emit sliderToPlot((double) (value * zoomMax / ui->zoomSlider->minimum()));
}

void PlotWidget::changeHScrollBarValue(QCPRange newRange, QCPRange completeRange) {
    //Umrechnung von QCPRange to int und verschieben der ScrollBar!
    int currentMin = ui->hScrollBar->minimum();
    int currentMax = ui->hScrollBar->maximum();
    //change range if needed:
    if( (currentMin != std::round(-completeRange.upper*scrollFaktor)) | (currentMax != std::round(-completeRange.lower*scrollFaktor)) ) {
        ui->hScrollBar->setRange(std::round(-completeRange.upper*scrollFaktor), std::round(-completeRange.lower*scrollFaktor));
    }
    //change pagestep of scrollbar if needed: after zoom change
    if((currentMax-currentMin)*scrollFaktor != (newRange.size() * scrollFaktor)) {
        ui->hScrollBar->setPageStep(std::round(newRange.size() * scrollFaktor));
    }
    //change position of scrollbar
    if(ui->hScrollBar->value() != std::round(-newRange.center()*scrollFaktor)) {
        ui->hScrollBar->setValue(std::round(-newRange.center()*scrollFaktor));
    }
}

void PlotWidget::changeVScrollBarValue(QCPRange newRange, QCPRange completeRange) {
    //Umrechnung von QCPRange to int und verschieben der ScrollBar!
    int currentMin = ui->vScrollBar->minimum();
    int currentMax = ui->vScrollBar->maximum();
    //change range if needed:
    if( (currentMin != std::round(-completeRange.upper*scrollFaktor)) | (currentMax != std::round(-completeRange.lower*scrollFaktor)) ) {
        ui->vScrollBar->setRange(std::round(-completeRange.upper*scrollFaktor), std::round(-completeRange.lower*scrollFaktor));
    }
    //change pagestep of scrollbar if needed: after zoom change
    if((currentMax-currentMin)*scrollFaktor != (newRange.size() * scrollFaktor)) {
        ui->vScrollBar->setPageStep(std::round(newRange.size() * scrollFaktor));
    }
    //change position of scrollbar
    if(ui->vScrollBar->value() != std::round(-newRange.center()*scrollFaktor)) {
        ui->vScrollBar->setValue(std::round(-newRange.center()*scrollFaktor));
    }
}

void PlotWidget::changeSliderPos(QCPRange xNow, QCPRange xComplete) {
    int newValue = sliderMapToValue(xNow, xComplete);

    if( ! (ui->zoomSlider->value() == newValue ) ) {
        ui->zoomSlider->setValue(newValue);

    }
}

 int PlotWidget::sliderMapToValue(QCPRange current, QCPRange complete) {
     //for changes in the mapping also change the slot: sliderPosChanged(int value) correspondingly.
     double ratio = current.size() / complete.size();

    double zoomMin = zoomMax / (ui->zoomSlider->maximum()-ui->zoomSlider->minimum());

    if(ratio > zoomMax) {
        ratio = zoomMax;
    } else if(ratio < zoomMin) {
        ratio = zoomMin;
    }

    int minimum = ui->zoomSlider->minimum(); //negative
    double value = std::round(minimum / zoomMax * ratio);

    return value;

}














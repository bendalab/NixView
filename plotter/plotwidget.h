#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <nix.hpp>
#include <QVariant>
#include <QModelIndex>
#include "plotter/plotter.h"
#include "views/MainViewWidget.hpp"
#include "utils/entitydescriptor.h"

namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

    bool canDraw() const;
    void setEntity(QVariant item);

    void clear();

    Plotter* process(const nix::DataArray &array);
    void process(const nix::MultiTag &mtag, nix::ndsize_t ref=0);
    void process(const nix::Tag &tag, nix::ndsize_t ref=0);
    void process(const nix::Feature &feat, const nix::Tag & tag);
    void process(const nix::Feature &feat, const nix::MultiTag & mtag);

public slots:
    void showMore();
    void savePlot();
    // slots and signals for the scrollbars and the zoom slider:
    void hScrollBarPosChanged(int value);   // emit signal to be caught by the plotter.
    void vScrollBarPosChanged(int value);
    void sliderPosChanged(int value);

    void changeHScrollBarValue(QCPRange newRange, QCPRange completeRange); // react to signals from the plotter.
    void changeVScrollBarValue(QCPRange newRange, QCPRange completeRange);
    void changeSliderPos(QCPRange xNow, QCPRange xComplete);

signals:
    void hScrollBarToPlot(double); // signals for the plotter.
    void vScrollBarToPlot(double);
    void sliderToPlot(double);

private:
    Ui::PlotWidget *ui;
    QVariant item;
    Plotter *plot;
    QString text;
    double scrollFaktor;
    double zoomFaktor;
    double zoomMax;

    bool checkPlottableDType(nix::DataType dtype) const;
    void deleteWidgetsFromLayout();
    void processItem();

    void draw1D(const nix::DataArray &array);
    void draw2D(const nix::DataArray &array);
    void drawMultiLine(const nix::DataArray &array);

    int sliderMapToValue(QCPRange current, QCPRange complete);
};

#endif // PLOTWIDGET_H

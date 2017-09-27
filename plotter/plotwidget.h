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

    bool can_draw() const;
    void setEntity(QVariant item);

    void clear();

    Plotter* process(const nix::DataArray &array);
    void process(const nix::MultiTag &mtag, nix::ndsize_t ref=0);
    void process(const nix::Tag &tag, nix::ndsize_t ref=0);
    void process(const nix::Feature &feat, const nix::Tag & tag);
    void process(const nix::Feature &feat, const nix::MultiTag & mtag);

public slots:
    void show_more();
    void save_plot();

    void vScrollBarPosChanged(int value);
    void hScrollBarPosChanged(int value);

    void changeHScrollBarValue(QCPRange newRange, QCPRange completeRange);
    void changeVScrollBarValue(QCPRange newRange, QCPRange completeRange);

signals:
    void vScrollValueChanged(QCPRange);
    void hScrollValueChanged(QCPRange);

private:
    Ui::PlotWidget *ui;
    QVariant item;
    Plotter *plot;
    QString text;

    bool check_plottable_dtype(nix::DataType dtype) const;
    void delete_widgets_from_layout();
    void process_item();

    void draw_1d(const nix::DataArray &array);
    void draw_2d(const nix::DataArray &array);
    void draw_multi_line(const nix::DataArray &array);
};

#endif // PLOTWIDGET_H

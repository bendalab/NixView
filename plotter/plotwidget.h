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
    void setEntity(QModelIndex qml);
    void savePlot();
    void clear();

    Plotter* process(const nix::DataArray &array);
    void process(const nix::MultiTag &mtag, nix::ndsize_t ref=0);
    void process(const nix::Tag &tag, nix::ndsize_t ref=0);

public slots:
    void show_more();

private:
    Ui::PlotWidget *ui;
    QModelIndex item_qml;
    NixTreeModelItem* item;
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

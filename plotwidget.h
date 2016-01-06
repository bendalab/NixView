#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <nix.hpp>
#include <QVariant>
#include <plotter/plotter.h>

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

    void setEntity(QVariant var);

private:
    Ui::PlotWidget *ui;
    QVariant item;
    std::vector<Plotter *> plots;

    void draw();
    bool check_plottable_dtype(nix::DataType dtype) const;
    void process_item();
    void delete_widgets_from_layout();
    void process(const nix::DataArray &array);
    void process(const nix::MultiTag &mtag);
    void process(const nix::Tag &tag);

    void draw_1d(const nix::DataArray &array);
    void draw_2d(const nix::DataArray &array);
};

#endif // PLOTWIDGET_H

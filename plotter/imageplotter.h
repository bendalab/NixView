#ifndef IMAGEPLOTTER_H
#define IMAGEPLOTTER_H

#include <QWidget>
#include "plotter/categoryplotter.h"
#include <nix.hpp>
#include "colormap.hpp"

namespace Ui {
    class ImagePlotter;
}

class ImagePlotter : public QWidget, public Plotter {
Q_OBJECT
public:

    virtual void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name,
                            bool y_scale) override;

    virtual void add_segments(const QVector<double> &positions, const QVector<double> &extents,
                              const QString &name) override;

    virtual void set_label(const std::string &label) override;

    virtual PlotterType plotter_type() const override;


public:
    explicit ImagePlotter(QWidget *parent = 0);
    ~ImagePlotter();

    void draw(const nix::DataArray &array);

private:
    Ui::ImagePlotter *ui;
    ColorMap cmap;

    QCustomPlot* get_plot() override;
};

#endif //IMAGEPLOTTER_H

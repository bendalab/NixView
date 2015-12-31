#ifndef CATEGORYPLOTTER_H
#define CATEGORYPLOTTER_H

#include <QWidget>
#include "plotter.h"

namespace Ui {
class CategoryPlotter;
}

class CategoryPlotter : public QWidget, public Plotter {
    Q_OBJECT

public:
    explicit CategoryPlotter(QWidget *parent = 0);
    ~CategoryPlotter();

    void set_label(const std::string &label);

    void set_ylabel(const QString &label);

    void set_xlabel(const QString &label);

    // void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale);

    // void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name);

    PlotterType plotter_type() const;

private:
    Ui::CategoryPlotter *ui;
    QCustomPlot* get_plot();

};

#endif // CATEGORYPLOTTER_H

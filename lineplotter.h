#ifndef LINEPLOTTER_H
#define LINEPLOTTER_H

#include <QWidget>
#include "Plotter/plotter.h"

namespace Ui {
    class LinePlotter;
}

class LinePlotter : public QWidget, public Plotter {
    Q_OBJECT

public:
    explicit LinePlotter(QWidget *parent = 0);
    ~LinePlotter();

    void set_label(const std::string &label);

    void add_line_plot(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name);

    void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale);

    void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name);

    PlotterType plotter_type() const;

private:
    Ui::LinePlotter *ui;
    QCustomPlot* get_plot();

};

#endif // LINEPLOTTER_H

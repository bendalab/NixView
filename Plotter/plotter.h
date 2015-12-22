#ifndef NIXVIEW_PLOTTER_H
#define NIXVIEW_PLOTTER_H

#include <QVariant>
#include <QSignalMapper>
#include <qcustomplot.h>
#include <QWidget>
#include <nix.hpp>

namespace Ui {
    class Plotter;
}

class Plotter : public QWidget {
       Q_OBJECT

public:
    Plotter(QWidget *parent);

    void add_events(const QVector<double> x_data, const QVector<double> y_data, const QString &name, bool y_scale=true);
    void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name);

    void data_array_to_qvector(const nix::DataArray &array, QVector<double> &xdata, QVector<double> &ydata, QVector<QString> &xticklabels, nix::ndsize_t dim_index);
    void data_array_ax_labels(const nix::DataArray &array, QString &ylabel, QVector<QString> &labels);    
};

#endif // NIXVIEW_PLOTTER_H

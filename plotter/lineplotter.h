#ifndef LINEPLOTTER_H
#define LINEPLOTTER_H

#include <QWidget>
#include "plotter.h"
#include <nix.hpp>
#include "colormap.hpp"

namespace Ui {
    class LinePlotter;
}

class LinePlotter : public QWidget, public Plotter {
    Q_OBJECT

public:
    explicit LinePlotter(QWidget *parent = 0, int numofPoints = 50000);
    ~LinePlotter();

    void draw(const nix::DataArray &array);

    void draw_1d(const nix::DataArray &array);

    void draw_2d(const nix::DataArray &array);

    bool check_dimensions(const nix::DataArray &array) const;

    int guess_best_xdim(const nix::DataArray &array) const; //tries to find the best x-dimension needs to be optional at some point...

    void set_label(const std::string &label) override;

    void add_line_plot(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name);

    void set_ylabel(const std::string &label);
    void set_ylabel(const QString &label);

    void set_xlabel(const QString &label);
    void set_xlabel(const std::string &label);

    void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) override;

    void add_events(const QVector<double> &x_data, const QString &name, bool y_scale);

    void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) override;

    PlotterType plotter_type() const override;

    void save(QString filename) {}

private:
    Ui::LinePlotter *ui;
    ColorMap cmap;
    int numOfPoints;
    QCPRange totalXRange;
    QCPRange totalYRange;

    QCustomPlot* get_plot() override;
    void setXRange(QVector<double> xData);
    void setYRange(QVector<double> yData);

signals:
    void xAxisChanged(QCPRange, QCPRange);
    void yAxisChanged(QCPRange, QCPRange);

public slots:
    void xAxisNewRange(QCPRange newRange);
    void yAxisNewRange(QCPRange newRange);

    void changeXAxisRange(QCPRange range);
    void changeYAxisRange(QCPRange range);


    void selection_changed();
    void mouse_wheel();
    void mouse_press();
    void show_legend();
    void context_menu_request(QPoint pos);
    void remove_selected_graph();
    void clear_selection();
    void set_pen_style(QString style);
    void set_pen_none();
    void set_pen_solid();
    void set_pen_dashed();
    void set_pen_dashdotted();
    void set_pen_dotted();
    void set_marker(QString marker);
    void marker_none();
    void marker_square();
    void marker_diamond();
    void marker_circle();
    void marker_cross();
    void marker_dot();
    void marker_plus();

};

#endif // LINEPLOTTER_H

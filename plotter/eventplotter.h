#ifndef EVENTPLOTTER_H
#define EVENTPLOTTER_H

#include <QWidget>
#include "plotter.h"
#include <nix.hpp>
#include "colormap.hpp"

namespace Ui {
    class EventPlotter;
}

class EventPlotter : public QWidget , public Plotter {
    Q_OBJECT

public:
    explicit EventPlotter(QWidget *parent = 0);
    ~EventPlotter();

    void draw(const QVector<QVector<double>> &positions, const QVector<QVector<QString>> &names);
    void draw(const QVector<QVector<double>> &positions, const QVector<QVector<double>> &extends , const QVector<QVector<QString>> &names);

    bool check_dimension(const nix::DataArray &array) const;

    void set_label(const std::string &label) override;

    void set_ylabel(const std::string &label);
    void set_ylabel(const QString &label);

    void set_xlabel(const QString &label);
    void set_xlabel(const std::string &label);

    void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale) override;

    void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) override;

    PlotterType plotter_type() const override;

    void save(QString filename) {}

    QCustomPlot* get_plot() override;

private:
    Ui::EventPlotter *ui;
    ColorMap cmap;

    //bool checkDimensions(QVector<QVector<double>> &positions, QVector<QVector<double>> &extends);
    //bool checkDimensions(QVector<QVector<double>> &positions);
    void plot(const QVector<double> &positions);
    void plot(const QVector<double> &positions, const QVector<double> &extends);

public slots:
    /*void selection_changed();
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
    void marker_plus();*/
};

#endif // EVENTPLOTTER_H

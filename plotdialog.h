#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QSignalMapper>
#include <qcustomplot.h>
#include <nix.hpp>

namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = 0);

    void set_entity(QVariant var);

    ~PlotDialog();

public slots:
    void show_context_menu();
    void show_legend();
    void mouse_press();
    void mouse_wheel();
    void selection_changed();
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

private:
    Ui::PlotDialog *ui;
    QVariant item;

    void draw();
    bool can_draw();
    void draw_data_array(const nix::DataArray &array);
    void draw_tag(const nix::Tag &tag);
    void draw_multi_tag(const nix::MultiTag &mtag);
    void draw_1d(const nix::DataArray &array);
    void draw_2d(const nix::DataArray &array);
    void draw_segment(const nix::Tag &tag);
    bool check_plottable_dtype(const nix::DataArray & array);

    void add_line_plot(QVector<double> x_data, QVector<double> y_data, QString name);
    void add_scatter_plot(QVector<double> x_data, QVector<double> y_data, QString name, bool y_scale=true);
    void add_bar_plot(QVector<QString> categories, QVector<double> y_data, QString name);

    void data_array_ax_labels(const nix::DataArray &array, QString &ylabel, QVector<QString> &labels);
    void data_array_to_qvector(const nix::DataArray &array, QVector<double> &xdata, QVector<double> &ydata, QVector<QString> &xlabels, nix::ndsize_t dim_index);
    QCustomPlot* get_plot();

};

#endif // PLOTDIALOG_H

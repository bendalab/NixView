#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QVariant>
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
    void vertical_zoom_clicked();
    void horizontal_zoom_clicked();
    void show_legend();

private:
    Ui::PlotDialog *ui;
    QVariant item;

    void draw();
    bool can_draw();
    void draw_1d(const nix::DataArray &array);
    void draw_2d(const nix::DataArray &array);
    QCustomPlot* get_plot();

};

#endif // PLOTDIALOG_H

#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
#include <nix.hpp>
#include <QVariant>
#include <plotter/plotter.h>
#include <QModelIndex>
#include "MainViewWidget.hpp"
#include <entitydescriptor.h>

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

private:
    Ui::PlotWidget *ui;
    QModelIndex item_qml;
    NixModelItem* item;

    bool check_plottable_dtype(nix::DataType dtype) const;
    void delete_widgets_from_layout();
    void process_item();
    Plotter* process(const nix::DataArray &array);
    void process(const nix::MultiTag &mtag);
    void process(const nix::Tag &tag);

    EntityDescriptor basic_description(const std::string &name, const std::string &type, const std::string &description,
                              const std::string &id, const std::string &created, const std::string &updated);
    void describe(const nix::DataArray &array);
    void describe(const nix::MultiTag &mtag);
    void describe(const nix::Tag &tag);

    void draw_1d(const nix::DataArray &array);
    void draw_2d(const nix::DataArray &array);
    void draw_multi_line(const nix::DataArray &array);
};

#endif // PLOTWIDGET_H

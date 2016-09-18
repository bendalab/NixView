#ifndef TAGVIEW_H
#define TAGVIEW_H

#include <QScrollArea>
#include <nix.hpp>
#include <QModelIndex>
#include <QListWidget>
#include "model/NixModelItem.hpp"
#include "utils/entitydescriptor.h"

namespace Ui {
class TagView;
}

class PlotWidget;

class TagView : public QScrollArea
{
    Q_OBJECT

public:
    explicit TagView(QWidget *parent = 0);
    ~TagView();

    void setEntity(QModelIndex qml);
    void clear();

public slots:
    void reference_selected();
    void feature_selected();
    void header_clicked();

private:
    Ui::TagView *ui;
    nix::Tag tag;

    void fill_references();
    void fill_features();
    void clear_references();
    void clear_features();
    void clear_plot_widget(PlotWidget *plot_widget);
    void plot_data(PlotWidget *plot_widget, const nix::DataArray &array);
};

#endif // TAGVIEW_H

#ifndef TAGVIEW_H
#define TAGVIEW_H

#include <QScrollArea>
#include <nix.hpp>
#include <QModelIndex>
#include <QMap>
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
    void reference_selected(int i);
    void feature_selected(int i);
    void header_clicked();

private:
    Ui::TagView *ui;
    nix::Tag tag;
    QMap<int, int> feature_map, reference_map;

    void fill_references();
    void fill_features();
    void clear_references();
    void clear_features();
};

#endif // TAGVIEW_H

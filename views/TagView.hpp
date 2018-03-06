#ifndef TAGVIEW_H
#define TAGVIEW_H

#include <QScrollArea>
#include <nix.hpp>
#include <QModelIndex>
#include <QMap>
#include <QListWidget>
#include "model/NixModelItem.hpp"
#include "utils/entitydescriptor.h"
#include "utils/tagcontainer.h"

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

    void setEntity(QVariant var);
    void clear();

public slots:
    void reference_selected(int i);
    void feature_selected(int i);
    void show_tag_info();

private:
    Ui::TagView *ui;
    QMap<int, int> feature_map, reference_map;
    TagContainer tag;

    void fill_references();
    void fill_features();
    void clear_references();
    void clear_features();
};

#endif // TAGVIEW_H

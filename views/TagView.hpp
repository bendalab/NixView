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

private:
    Ui::TagView *ui;
    nix::Tag tag;

    EntityDescriptor basic_description(const std::string &name, const std::string &type, const std::string &description,
                                       const std::string &id, const std::string &created, const std::string &updated);
    void fill_references();
    void fill_features();
    void clear_plot_widget(QWidget *plot_widget);
    void plot_data(QWidget *plot_widget, const nix::DataArray &array);
};

#endif // TAGVIEW_H

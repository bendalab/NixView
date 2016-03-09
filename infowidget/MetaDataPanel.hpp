#ifndef METADATAPANEL_HPP
#define METADATAPANEL_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>
#include <QSortFilterProxyModel>
#include "model//NixDataModel.hpp"
#include "filter/NixProxyModel.hpp"

namespace Ui {
class MetaDataPanel;
}

class MetaDataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataPanel(NixDataModel *_nix_model, QWidget *parent = 0);
    ~MetaDataPanel();

public slots:
//    void update_metadata_panel(QVariant);
    void resize_to_content(QModelIndex);

public:
    const QTreeView* get_tree_view();
    void update_metadata_panel(QModelIndex qml);
    void clear_metadata_panel();

private:
    NixProxyModel* proxy_model;
    void set_proxy_model();

    void add_children_to_item(QTreeWidgetItem*, nix::Section);
    void add_properties_to_item(QTreeWidgetItem*, nix::Section);
    void add_values_to_property(QTreeWidgetItem*, nix::Property);

    Ui::MetaDataPanel *ui;
};

#endif // METADATAPANEL_HPP

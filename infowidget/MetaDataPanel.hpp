#ifndef METADATAPANEL_HPP
#define METADATAPANEL_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>
#include "model/nixtreemodel.h"
#include "filter/NixProxyModel.hpp"

namespace Ui {
class MetaDataPanel;
}

class MetaDataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataPanel(QWidget *parent = 0);
    ~MetaDataPanel();

    void setDataModel(NixTreeModel *_nix_model);
public slots:
    void resize_to_content(QModelIndex);
    void set_column_state(QString column, bool visible);

public:
    QTreeView* get_tree_view();
    void update_metadata_panel(QModelIndex qml);
    void clear_metadata_panel();

private:
    NixProxyModel* proxy_model;
    void set_proxy_model();
    void set_columns();
    void add_children_to_item(QTreeWidgetItem*, nix::Section);
    void add_properties_to_item(QTreeWidgetItem*, nix::Section);
    void add_values_to_property(QTreeWidgetItem*, nix::Property);

    Ui::MetaDataPanel *ui;
};

#endif // METADATAPANEL_HPP

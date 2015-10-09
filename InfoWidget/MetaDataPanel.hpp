#ifndef METADATAPANEL_HPP
#define METADATAPANEL_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>

namespace Ui {
class MetaDataPanel;
}

class MetaDataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataPanel(QWidget *parent = 0);
    ~MetaDataPanel();

public slots:
    void update_metadata_panel(nix::Section);
    void resize_to_content(QModelIndex);

public:
    const QTreeWidget* get_tree_widget();

private:
    void add_children_to_item(QTreeWidgetItem*, nix::Section);
    void add_properties_to_item(QTreeWidgetItem*, nix::Section);

    Ui::MetaDataPanel *ui;
};

#endif // METADATAPANEL_HPP

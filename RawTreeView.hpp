#ifndef RAWTREEVIEW_HPP
#define RAWTREEVIEW_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>
#include <QVariant>
#include <QComboBox>
#include "NixDataModel/NixDataModel.hpp"

namespace Ui {
class RawTreeView;
}

class RawTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit RawTreeView(NixDataModel* nix_model, QWidget *parent = 0);
    ~RawTreeView();

public slots:
//    void currentItemChanged_worker(QTreeWidgetItem*,QTreeWidgetItem*);
//    void resize_to_content(QModelIndex);
//    void item_info_requested(QTreeWidgetItem*, int);
//    void filter_changed(QString);

signals:
    void item_found(QVariant);
    void empty_item();

private:
    /**
     * @brief filter_mode
     * 0 = no filter
     * 1 = data
     * 2 = metadata
     * 3 = data arrays
     * 4 = tags/multitags
     * 5 = sources
     */
    int filter_mode;

    Ui::RawTreeView* ui;
    nix::File nix_file;
    NixDataModel* nix_model;
    void init_tree_widget();
//    void add_linked_sources(QTreeWidgetItem*, QVariant);

//    template <typename T>
//    void add_linked_sources_helper(QTreeWidgetItem*, T);

//    static void add_children_to_item(QTreeWidgetItem* item, nix::Section section);

//    void refresh();
//    void expand();

//    // friend declaration and getters for signals and slots
//    friend class MainViewWidget;
//    const QTreeWidget* get_tree_widget();
//    const QComboBox* get_filter_combo_box();
};
#endif // RAWTREEVIEW_HPP

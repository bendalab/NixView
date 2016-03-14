#ifndef RAWTREEVIEW_HPP
#define RAWTREEVIEW_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>
#include <QVariant>
#include <QComboBox>
#include "model/NixDataModel.hpp"
#include "filter/NixProxyModel.hpp"

namespace Ui {
class RawTreeView;
}

class RawTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit RawTreeView(NixProxyModel *nix_proxy_model, QWidget *parent = 0);
    explicit RawTreeView(QWidget *parent = 0);
    ~RawTreeView();

public slots:
    void resize_to_content(QModelIndex);

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
    NixProxyModel *nix_proxy_model;
    std::vector<int> hidden_columns;

public:
    const QTreeView* get_tree_view();
    void  set_proxy_model(NixProxyModel *proxy_model);
//    const QComboBox* get_filter_combo_box();
};
#endif // RAWTREEVIEW_HPP

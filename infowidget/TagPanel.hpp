#ifndef TAGPANEL_HPP
#define TAGPANEL_HPP

#include <QWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QVariant>
#include <nix.hpp>
#include <QtGui>

namespace Ui {
class TagPanel;
}

class TagPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TagPanel(QWidget *parent = 0);
    ~TagPanel();

public slots:
    void update_tag_panel(QModelIndex qml);
    void reference_item_requested(QTreeWidgetItem*, int);
    void feature_item_requested(QTreeWidgetItem*, int);
    void tag_item_requested(int, int, int, int);
    void currentItemChanged_reference_helper(QTreeWidgetItem*,QTreeWidgetItem*);
    void currentItemChanged_feature_helper(QTreeWidgetItem*,QTreeWidgetItem*);

signals:
    void emit_reference(nix::DataArray);
    void references_cleared();
    void emit_feature(nix::DataArray);
    void features_cleared();
    void emit_tag(nix::Tag, int);

private:
    Ui::TagPanel *ui;
    std::string extract_tag_info(nix::Tag);
    void extract_multitag_info(nix::MultiTag);
    std::vector<nix::DataArray> references;
    std::vector<nix::DataArray> features;
    QModelIndex current_qml;
    static void fill_tree(QTreeWidget*, std::vector<nix::DataArray>);

// getter
public:
    void clear_tag_panel();
    QTreeWidget* get_reference_tree();
    QTreeWidget* get_feature_tree();
    QTableWidget* get_tag_table();
};

#endif // TAGPANEL_HPP

#ifndef TAGPANEL_HPP
#define TAGPANEL_HPP

#include <QWidget>
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
    void update_tag_panel(QVariant v);
    void reference_item_requested(QTreeWidgetItem*, int);
    void feature_item_requested(QTreeWidgetItem*, int);

signals:
    void item_found(nix::DataArray);

private:
    Ui::TagPanel *ui;
    void clear_tag_panel();
    std::string extract_tag_info(nix::Tag);
    void extract_multitag_info(nix::MultiTag);
    std::vector<nix::DataArray> references;
    std::vector<nix::DataArray> features;
    static void fill_tree(QTreeWidget*, std::vector<nix::DataArray>);

    // getter
public:
    QTreeWidget* get_reference_tree();
    QTreeWidget* get_feature_tree();
};

#endif // TAGPANEL_HPP

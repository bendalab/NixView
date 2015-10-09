#ifndef RAWTREEVIEW_HPP
#define RAWTREEVIEW_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>

namespace Ui {
class RawTreeView;
}

class RawTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit RawTreeView(nix::File nix_file, QWidget *parent = 0);
    ~RawTreeView();

public slots:
    void entry_double_clicked(QTreeWidgetItem* item, int column);
    void resize_to_content(QModelIndex);
    void item_info_requested(QTreeWidgetItem*, int);

signals:
//    void item_info_found(std::string, std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);
//    void item_info_found(std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);

    void item_found(QVariant);

private:
    Ui::RawTreeView *ui;
    nix::File nix_file;
    void init_tree_widget();
    static void add_children_to_item(QTreeWidgetItem* item, nix::Section section);

    // friend declaration and getters for signals and slots
    friend class MainViewWidget;
    const QTreeWidget* get_tree_widget();
};
#endif // RAWTREEVIEW_HPP

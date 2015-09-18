#ifndef RAWTREEVIEW_HPP
#define RAWTREEVIEW_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>

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

private:
    Ui::RawTreeView *ui;
    nix::File nix_file;
    void init_tree_widget();
    static void add_children_to_item(QTreeWidgetItem* item, nix::Section section);
};

#endif // RAWTREEVIEW_HPP

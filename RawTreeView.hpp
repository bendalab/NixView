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

private:
    Ui::RawTreeView *ui;
    static void add_children_to_item(QTreeWidgetItem* item, nix::Section section);
};

#endif // RAWTREEVIEW_HPP

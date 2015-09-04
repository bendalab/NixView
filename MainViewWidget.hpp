#ifndef MAINVIEWWIDGET_HPP
#define MAINVIEWWIDGET_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>

namespace Ui {
class MainViewWidget;
}

class MainViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainViewWidget(std::string& nix_file_path, QWidget *parent = 0);
    ~MainViewWidget();

private:
    Ui::MainViewWidget *ui;
    nix::File nix_file;

    void populate_data_stacked_widget();
    static void add_children_to_item(QTreeWidgetItem*, nix::Section);
};

#endif // MAINVIEWWIDGET_HPP

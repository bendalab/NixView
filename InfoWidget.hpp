#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>
#include <QtGui>
#include <boost/optional.hpp>
#include <nix.hpp>

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();

    bool is_active;

public slots:
    void update_info_widget(std::string, std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);
    void update_info_widget(std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);

    void resize_to_content(QModelIndex);

signals:
    void add_info_widget();

private:
    Ui::InfoWidget *ui;

    void update_meta_info(nix::Section);
    void add_children_to_item(QTreeWidgetItem*, nix::Section);

public:
    const QTreeWidget* get_tree_widget();

};

#endif // INFOWIDGET_HPP

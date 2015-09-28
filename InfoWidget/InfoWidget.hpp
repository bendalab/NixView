#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>
#include <QtGui>
#include <boost/optional.hpp>
#include <nix.hpp>
#include "DescriptionPanel.hpp"
#include "MetaDataPanel.hpp"

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();

public slots:
    void update_info_widget(std::string, std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);
    void update_info_widget(std::string, std::string, boost::optional<std::basic_string<char>>, nix::Section);

private:
    Ui::InfoWidget *ui;

    DescriptionPanel* dp;
    MetaDataPanel* mp;

    void update_meta_info(nix::Section);
    void add_children_to_item(QTreeWidgetItem*, nix::Section);
    void add_properties_to_item(QTreeWidgetItem*, nix::Section);
    void connect_widgets();

public:
    const MetaDataPanel* get_metadata_panel();

};

#endif // INFOWIDGET_HPP

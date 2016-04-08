#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>
#include <QTreeWidget>
#include <QtGui>
#include <QVariant>
#include <boost/optional.hpp>
#include <nix.hpp>
#include "DescriptionPanel.hpp"
#include "MetaDataPanel.hpp"
#include "TagPanel.hpp"
#include "TagView.hpp"
#include "model/NixDataModel.hpp"

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent =0);
    ~InfoWidget();

    void setDataModel(NixDataModel *_nix_model);

public slots:
    void update_info_widget(QModelIndex qml);

private:
    Ui::InfoWidget *ui;

    DescriptionPanel* dp;
    MetaDataPanel* mp;
    TagPanel* tp;
    TagView* tv;

    void update_meta_info(nix::Section);
    void add_children_to_item(QTreeWidgetItem*, nix::Section);
    void add_properties_to_item(QTreeWidgetItem*, nix::Section);
    void connect_widgets();

public:
    const MetaDataPanel* get_metadata_panel();


};
#endif // INFOWIDGET_HPP

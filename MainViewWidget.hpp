#ifndef MAINVIEWWIDGET_HPP
#define MAINVIEWWIDGET_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <infowidget/InfoWidget.hpp>
#include <boost/optional.hpp>
#include "views/RawTreeView.hpp"
#include "views/ColumnView.hpp"
#include "model/NixDataModel.hpp"

namespace Ui {
class MainViewWidget;
}

class MainViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainViewWidget(std::string& nix_file_path, QWidget *parent = 0);
    ~MainViewWidget();

    RawTreeView* get_rtv();

public slots:
    void set_view(int);
    void activate_info_widget();

private:
    Ui::MainViewWidget *ui;
    nix::File nix_file;
    NixDataModel* nix_model;

    RawTreeView* rtv;
    ColumnView* cv;
    InfoWidget* iw;

    void populate_data_stacked_widget();

    void connect_widgets();
};

#endif // MAINVIEWWIDGET_HPP

#ifndef MAINVIEWWIDGET_HPP
#define MAINVIEWWIDGET_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <RawTreeView.hpp>
#include <InfoWidget/InfoWidget.hpp>
#include <boost/optional.hpp>
#include "RawTreeView.hpp"
#include <NixDataModel/NixDataModel.hpp>

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
    InfoWidget* iw;

    void populate_data_stacked_widget();

    void connect_widgets();
};

#endif // MAINVIEWWIDGET_HPP

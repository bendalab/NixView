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
#include "filter/NixProxyModel.hpp"
#include "QShortcut"

namespace Ui {
class MainViewWidget;
}

class MainViewWidget : public QWidget
{
    Q_OBJECT

private:
    static NixDataModel *CURRENT_MODEL;

public:
    explicit MainViewWidget(QWidget *parent = 0);
    explicit MainViewWidget(const std::string &nix_file_path, QWidget *parent = 0);
    ~MainViewWidget();

    void set_nix_file(const std::string &nix_file_path);
    RawTreeView *get_rtv();
    ColumnView *get_cv();
    static NixDataModel* get_current_model() {return CURRENT_MODEL; }
    int get_scan_progress();
    void clear();

public slots:
    void set_view(int);
    void activate_info_widget();
    void emit_current_qml_worker_slot(QModelIndex qml, QModelIndex);
    void scan_progress();

signals:
    void emit_model_update(NixDataModel*);
    void scan_progress_update();
    void emit_current_qml(QModelIndex);

private:
    Ui::MainViewWidget *ui;
    nix::File nix_file;
    NixDataModel *nix_model;
    NixProxyModel *nix_proxy_model;

    RawTreeView *rtv;
    ColumnView *cv;
    InfoWidget *iw;

    QShortcut *shortcut_filter;

    void populate_data_stacked_widget();
    void connect_widgets();
};

#endif // MAINVIEWWIDGET_HPP

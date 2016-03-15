#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"
#include "common/Common.hpp"

NixDataModel *MainViewWidget::CURRENT_MODEL = nullptr;

MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    ui->setupUi(this);

    if (nix_file.isOpen())
        nix_file.close();

    nix_model = nullptr;
    iw = nullptr;
    rtv = nullptr;
    cv = nullptr;

    QStringList filter_expressions = {FILTER_EXP_NONE,
                                      FILTER_EXP_METADATA,
                                      FILTER_EXP_DATAARRAY,
                                      FILTER_EXP_TAG,
                                      FILTER_EXP_MULTITAG,
                                      FILTER_EXP_NAME_CONTAINS,
                                      FILTER_EXP_NIXTYPE_CONTAINS};
    ui->cmbx_filter->addItems(filter_expressions);
}

/**
* @brief Container for all widgets for data display.
* @param nix_file_path: path to opened nix file
*/
MainViewWidget::MainViewWidget(const std::string &nix_file_path, QWidget *parent) :
    MainViewWidget(parent)
{
    set_nix_file(nix_file_path);
}

void MainViewWidget::set_nix_file(const std::string &nix_file_path)
{
    nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);
    nix_model = new NixDataModel();
    connect(nix_model, SIGNAL(file_scan_progress()), this, SLOT(scan_progress()));
    nix_model->nix_file_to_model(nix_file);
    MainViewWidget::CURRENT_MODEL = nix_model;

    nix_proxy_model = new NixProxyModel();
    nix_proxy_model->setSourceModel(nix_model);
    nix_proxy_model->set_filter_mode(3);

    iw = new InfoWidget(nix_model, this);
    ui->horizontalLayout->addWidget(iw);

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    // order of adding views has to be consistent with integers defined in Common.hpp
    // 0
    rtv = new RawTreeView(nix_proxy_model, this);
    ui->data_stacked_Widget->addWidget(rtv);
    // 1
    cv = new ColumnView(nix_proxy_model, this);
    ui->data_stacked_Widget->addWidget(cv);

    ui->data_stacked_Widget->setCurrentIndex(0);
}

RawTreeView* MainViewWidget::get_rtv() {
    return rtv;
}

ColumnView* MainViewWidget::get_cv() {
    return cv;
}

// slots
void MainViewWidget::set_view(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}

void MainViewWidget::activate_info_widget()
{
    ui->horizontalLayout->addWidget(iw);
}

void MainViewWidget::emit_current_qml_worker_slot(QModelIndex qml, QModelIndex)
{
    emit emit_current_qml(nix_proxy_model->mapToSource(qml));
}

void MainViewWidget::scan_progress()
{
    emit scan_progress_update();
}

int MainViewWidget::get_scan_progress()
{
    return nix_model->progress();
}

// widget connection
void MainViewWidget::connect_widgets()
{
    // connections from views to current-index emitter
    QObject::connect(rtv->get_tree_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex,QModelIndex)));
    QObject::connect(cv->get_column_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex,QModelIndex)));

    // InfoWidget
    QObject::connect(this, SIGNAL(emit_current_qml(QModelIndex)), iw, SLOT(update_info_widget(QModelIndex)));

    // tree widget expanded/collapsed
    QObject::connect(rtv->get_tree_view(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(rtv->get_tree_view(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));

    // filter
    QObject::connect(ui->cmbx_filter, SIGNAL(currentIndexChanged(QString)), nix_proxy_model, SLOT(set_rough_filter(QString)));
    QObject::connect(ui->line_edit_filter, SIGNAL(textChanged(QString)), nix_proxy_model, SLOT(set_fine_filter(QString)));
    QObject::connect(ui->cbx_filter, SIGNAL(toggled(bool)), nix_proxy_model, SLOT(set_case_sensitivity(bool)));

    // ALSO CHECK CONNECTIONS IN InfoWidget.cpp
}

MainViewWidget::~MainViewWidget()
{
    if (nix_file.isOpen()) {
        nix_file.close();
    }
    delete ui;
}

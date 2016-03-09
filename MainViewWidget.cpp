#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"

NixDataModel *MainViewWidget::CURRENT_MODEL = nullptr;

MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    if (nix_file.isOpen())
        nix_file.close();

    nix_model = nullptr;
    iw = nullptr;
    rtv = nullptr;
    cv = nullptr;
}

/**
* @brief Container for all widgets for data display.
* @param nix_file_path: path to opened nix file
*/
MainViewWidget::MainViewWidget(std::string& nix_file_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    ui->setupUi(this);

    nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);
    nix_model = new NixDataModel(nix_file);
    MainViewWidget::CURRENT_MODEL = nix_model;

    nix_proxy_model = new NixProxyModel();
    nix_proxy_model->setSourceModel(nix_model);

    iw = new InfoWidget(nix_model, this);
    ui->horizontalLayout->addWidget(iw);

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    rtv = new RawTreeView(nix_proxy_model, this);
    ui->data_stacked_Widget->addWidget(rtv);
    cv = new ColumnView(nix_proxy_model, this);
    ui->data_stacked_Widget->addWidget(cv);

    ui->data_stacked_Widget->setCurrentIndex(1);
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

// widget connection
void MainViewWidget::connect_widgets()
{
    // connections from views to current-index emitter
    QObject::connect(rtv->get_tree_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex,QModelIndex)));
    QObject::connect(cv->get_column_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex,QModelIndex)));

    // - InfoWidget
    QObject::connect(this, SIGNAL(emit_current_qml(QModelIndex)), iw, SLOT(update_info_widget(QModelIndex)));

    // tree widget expanded/collapsed
    QObject::connect(rtv->get_tree_view(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(rtv->get_tree_view(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));

    // ALSO CHECK CONNECTIONS IN InfoWidget.cpp
}

MainViewWidget::~MainViewWidget()
{
    if (nix_file.isOpen()) {
        nix_file.close();
    }
    delete ui;
}

#include "views/MainViewWidget.hpp"
#include "ui_MainViewWidget.h"
#include "common/Common.hpp"
#include "model/nixtreemodel.h"

NixTreeModel *MainViewWidget::CURRENT_MODEL = nullptr;

MainViewWidget::MainViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    ui->setupUi(this);

    if (nix_file.isOpen())
        nix_file.close();

    tv = nullptr;
    nix_model = nullptr;
    nix_proxy_model = nullptr;
    iw = nullptr;
    cv = nullptr;
    QStringList filter_expressions = {FILTER_EXP_NONE,
                                      FILTER_EXP_BLOCK,
                                      FILTER_EXP_GROUP,
                                      FILTER_EXP_METADATA,
                                      FILTER_EXP_DATAARRAY,
                                      FILTER_EXP_TAG,
                                      FILTER_EXP_MULTITAG,
                                      FILTER_EXP_NAME_CONTAINS,
                                      FILTER_EXP_NIXTYPE_CONTAINS};
    ui->cmbx_filter->addItems(filter_expressions);
    ui->find_widget->setVisible(false);
    shortcut_filter = new QShortcut(QKeySequence("Ctrl+f"), this);
    populate_data_stacked_widget();
    connect_widgets();
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


void MainViewWidget::set_nix_file(const std::string &nix_file_path) {
    if (tv == nullptr)
        populate_data_stacked_widget();
    nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);

    nix_model = new NixTreeModel(this);
    nix_model->set_entity(nix_file);
    MainViewWidget::CURRENT_MODEL = nix_model;
    nix_proxy_model = new NixProxyModel(this);
    nix_proxy_model->setSourceModel(nix_model);
    tv->getTreeView()->setModel(nix_proxy_model);
    tv->getTreeView()->setSortingEnabled(true);
    cv->set_proxy_model(nix_proxy_model);
    emit emit_model_update(nix_model);
    QObject::connect(tv->getTreeView(), SIGNAL(clicked(QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex)));
    QObject::connect(cv->get_column_view(), SIGNAL(clicked(QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex)));
    QObject::connect(ui->cmbx_filter, SIGNAL(currentIndexChanged(QString)), nix_proxy_model, SLOT(set_rough_filter(QString)));
    QObject::connect(ui->line_edit_filter, SIGNAL(textChanged(QString)), nix_proxy_model, SLOT(set_fine_filter(QString)));
    QObject::connect(ui->cbx_filter, SIGNAL(toggled(bool)), nix_proxy_model, SLOT(set_case_sensitivity(bool)));
}


void MainViewWidget::clear() {
    nix_model = nullptr;
    nix_proxy_model = nullptr;
    emit emit_model_update(nix_model);
    delete cv;
    cv = nullptr;
    delete tv;
    tv = nullptr;
}


void MainViewWidget::populate_data_stacked_widget() {
    // order of adding views has to be consistent with integers defined in Common.hpp
    // 0
    tv = new LazyLoadView();
    ui->data_stacked_Widget->addWidget(tv);
    // 1
    cv = new ColumnView(this);
    ui->data_stacked_Widget->addWidget(cv);
    ui->data_stacked_Widget->setCurrentIndex(0);
}


/*
RawTreeView* MainViewWidget::get_rtv() {
    return rtv;
}
*/


ColumnView* MainViewWidget::get_cv() {
    return cv;
}

LazyLoadView *MainViewWidget::getTreeView() {
    return tv;
}


// slots
void MainViewWidget::set_view(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}


void MainViewWidget::activate_info_widget() {
    ui->horizontalLayout->addWidget(iw);
}


void MainViewWidget::emit_current_qml_worker_slot(QModelIndex qml) {
    emit emit_current_qml(nix_proxy_model->mapToSource(qml));
}


void MainViewWidget::scan_progress() {
    emit scan_progress_update();
}


int MainViewWidget::get_scan_progress() {
    //return nix_model->progress();
    return 100;
}


void MainViewWidget::toggle_find() {
    ui->find_widget->setVisible(!ui->find_widget->isVisible());
}

// widget connection
void MainViewWidget::connect_widgets() {
    // tree widget expanded/collapsed
    //QObject::connect(rtv->get_tree_view(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    //QObject::connect(rtv->get_tree_view(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    //QObject::connect(rtv->get_tree_view(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(set_current_depth_expanded(QModelIndex)));
    //QObject::connect(rtv->get_tree_view(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(set_current_depth_collapsed(QModelIndex)));

    // filter
    //QObject::connect(ui->cmbx_filter, SIGNAL(currentIndexChanged(QString)), rtv, SLOT(expand_collapse(QString)));
    //QObject::connect(ui->line_edit_filter, SIGNAL(textChanged(QString)), rtv, SLOT(expand_collapse(QString)));
    //QObject::connect(ui->cbx_filter, SIGNAL(toggled(bool)), rtv, SLOT(expand_collapse(bool)));
    QObject::connect(shortcut_filter, SIGNAL(activated()), ui->line_edit_filter, SLOT(setFocus()));
}


MainViewWidget::~MainViewWidget() {
    if (nix_file.isOpen()) {
        nix_file.close();
    }
    delete ui;
}

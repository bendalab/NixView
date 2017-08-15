#include <QMessageBox>
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
    populate_data_stacked_widget();
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

bool MainViewWidget::set_nix_file(const QString &nix_file_path) {
    return set_nix_file(nix_file_path.toStdString());
}

bool MainViewWidget::set_nix_file(const std::string &nix_file_path) {
    bool result = false;
    if (tv == nullptr)
        populate_data_stacked_widget();
    if (nix_file_path.empty()) {
        return result;
    }
    nix_model = new NixTreeModel(this);
    nix_proxy_model = new NixProxyModel(this);

    if (!nix_file.isNone() && nix_file.isOpen()) {
        nix_file.close();
    }
    if(!nix_file_path.empty()) {
        try {
            nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);
            nix_model->set_entity(nix_file);
            tv->getTreeView()->setModel(nix_proxy_model);
            tv->getTreeView()->setSortingEnabled(true);
            tv->setColumns();
            emit emit_model_update(nix_model);
            emit update_file(QString::fromStdString(nix_file_path));
            QObject::connect(tv->getTreeView(), SIGNAL(clicked(QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex)));
            QObject::connect(tv->getTreeView(), SIGNAL(expanded(QModelIndex)), tv, SLOT(resizeRequest()));
            QObject::connect(tv->getTreeView(), SIGNAL(collapsed(QModelIndex)), tv, SLOT(resizeRequest()));
            QObject::connect(tv->getTreeView()->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex, QModelIndex)));
            QObject::connect(cv->get_column_view(), SIGNAL(clicked(QModelIndex)), this, SLOT(emit_current_qml_worker_slot(QModelIndex)));
            result = true;
        } catch (const std::exception& e) {
            QMessageBox::information(this, QString::fromStdString("Error reading file " + nix_file_path + "!"),
                                     e.what(), QMessageBox::Ok);
        }
    } else {
        emit emit_model_update(nix_model);
        emit update_file(QString(""));
    }
    nix_proxy_model->setSourceModel(nix_model);
    MainViewWidget::CURRENT_MODEL = nix_model;
    cv->set_proxy_model(nix_proxy_model);
    return result;
}


bool MainViewWidget::set_project(const QString &project) {
    return ui->project_navigator->set_project(project);
}


void MainViewWidget::new_project() {
   ui->project_navigator->new_project();
}


void MainViewWidget::clear() {
    nix_model = nullptr;
    nix_proxy_model = nullptr;
    emit emit_model_update(nix_model);
    ui->project_navigator->clear();
    delete cv;
    cv = nullptr;
    delete tv;
    tv = nullptr;
    populate_data_stacked_widget();
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


ColumnView* MainViewWidget::get_cv() {
    return cv;
}


LazyLoadView *MainViewWidget::getTreeView() {
    return tv;
}


nix::File MainViewWidget::get_nix_file() const {
    return this->nix_file;
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
    tv->resizeRequest();
}


void MainViewWidget::emit_current_qml_worker_slot(QModelIndex qml, QModelIndex prev) {
    emit_current_qml_worker_slot(qml);
}

void MainViewWidget::scan_progress() {
    emit scan_progress_update();
}

void MainViewWidget::update_nix_file(const QString &nix_file_path) {
    set_nix_file(nix_file_path);
}

void MainViewWidget::project_add_file() {
    this->ui->project_navigator->add_file();
}

void MainViewWidget::project_remove_file() {
    this->ui->project_navigator->remove_file();
}

void MainViewWidget::close_nix_file() {
    this->set_nix_file(QString(""));
    //emit close_file();
}

int MainViewWidget::get_scan_progress() {
    //return nix_model->progress();
    return 100;
}

void MainViewWidget::show_project_navigator(bool show) {
    if (!show) {
        ui->splitter->setSizes({0, 100});
    } else {
        ui->splitter->setSizes({120,100});
    }
}

MainViewWidget::~MainViewWidget() {
    if (nix_file.isOpen()) {
        nix_file.close();
    }
    delete ui;
}

#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"

NixDataModel *MainViewWidget::CURRENT_MODEL = nullptr;

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

    iw =  new InfoWidget(nix_model, this);
    ui->horizontalLayout->addWidget(iw);

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    rtv = new RawTreeView(nix_model, this);
    ui->data_stacked_Widget->addWidget(rtv);
    cv = new ColumnView(nix_model, this);
    ui->data_stacked_Widget->addWidget(cv);

    ui->data_stacked_Widget->setCurrentIndex(0);
}

RawTreeView* MainViewWidget::get_rtv() {
    return rtv;
}

// slots
void MainViewWidget::set_view(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}

void MainViewWidget::activate_info_widget()
{
    ui->horizontalLayout->addWidget(iw);
}

// widget connection
void MainViewWidget::connect_widgets()
{
    // click in overview
    // - rawtreeview
    QObject::connect(rtv->get_tree_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), rtv, SLOT(current_changed(QModelIndex,QModelIndex)));
//    QObject::connect(rtv->get_tree_view()->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
//                     rtv, SLOT(selection_changed(QItemSelection,QItemSelection)));
//    QObject::connect(rtv, SIGNAL(item_found(QVariant)), iw, SLOT(update_info_widget(QVariant)));
//    QObject::connect(rtv, SIGNAL(empty_item()), iw, SLOT(update_info_widget()));
//    QObject::connect(rtv->get_filter_combo_box(), SIGNAL(activated(QString)), rtv, SLOT(filter_changed(QString)));

    // - InfoWidget
    QObject::connect(rtv->get_tree_view()->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), iw, SLOT(update_info_widget(QModelIndex,QModelIndex)));

    // - tag references

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

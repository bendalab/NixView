#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"


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

    iw =  new InfoWidget();
    ui->horizontalLayout->addWidget(iw);

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    rtv = new RawTreeView(nix_model);
    ui->data_stacked_Widget->addWidget(rtv);
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
//    QObject::connect(rtv->get_tree_widget(), SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
//                     rtv, SLOT(currentItemChanged_worker(QTreeWidgetItem*, QTreeWidgetItem*)));
//    QObject::connect(rtv, SIGNAL(item_found(QVariant)), iw, SLOT(update_info_widget(QVariant)));
//    QObject::connect(rtv, SIGNAL(empty_item()), iw, SLOT(update_info_widget()));
//    QObject::connect(rtv->get_filter_combo_box(), SIGNAL(activated(QString)), rtv, SLOT(filter_changed(QString)));

//    // - tag references

//    // tree widget expanded/collapsed
//    QObject::connect(rtv->get_tree_widget(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
//    QObject::connect(rtv->get_tree_widget(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));

    // ALSO CHECK CONNECTIONS IN InfoWidget.cpp
}

MainViewWidget::~MainViewWidget()
{
    if (nix_file.isOpen()) {
        nix_file.close();
    }
    delete ui;
}

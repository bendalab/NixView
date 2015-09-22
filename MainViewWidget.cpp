#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"
#include "RawTreeView.hpp"
#include "AnotherTree.hpp"


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

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    rtv = new RawTreeView(nix_file);
    ui->data_stacked_Widget->addWidget(rtv);

    iw =  new InfoWidget();

    // for testing
    AnotherTree* aot  = new AnotherTree();
    ui->data_stacked_Widget->addWidget(aot);
    // =====
}

// slots
void MainViewWidget::data_combo_box_changed(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}

void MainViewWidget::activate_info_widget()
{
    ui->horizontalLayout->addWidget(iw);
//    iw->setSizePolicy();
}

// widget connection
void MainViewWidget::connect_widgets()
{
    // view mode combo box
    QObject::connect(ui->data_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(data_combo_box_changed(int)));

    // activate info widget
    QObject::connect(iw, SIGNAL(add_info_widget()), this, SLOT(activate_info_widget()));

    // double click in overview
    QObject::connect(rtv->get_tree_widget(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), iw, SLOT(update_info_widget(QTreeWidgetItem* ,int)));
    QObject::connect(rtv->get_tree_widget(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(rtv->get_tree_widget(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
}

MainViewWidget::~MainViewWidget()
{
    delete ui;
}

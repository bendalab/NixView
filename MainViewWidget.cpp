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
    QObject::connect(ui->data_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(data_combo_box_changed(int)));

    nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);

    populate_data_stacked_widget();
}

void MainViewWidget::populate_data_stacked_widget()
{
    RawTreeView* rtv = new RawTreeView(nix_file);
    ui->data_stacked_Widget->addWidget(rtv);

    AnotherTree* aot  = new AnotherTree();
    ui->data_stacked_Widget->addWidget(aot);
//    std::cout << ui->data_stacked_Widget->currentWidget()->objectName().toStdString() << std::endl;
}

void MainViewWidget::data_combo_box_changed(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}

MainViewWidget::~MainViewWidget()
{
    delete ui;
}

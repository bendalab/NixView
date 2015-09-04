#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"
#include "RawTreeView.hpp"


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
}

void MainViewWidget::populate_data_stacked_widget()
{
    RawTreeView* rtv = new RawTreeView(nix_file);
    ui->data_stacked_Widget->addWidget(rtv);
    std::cout << ui->data_stacked_Widget->currentIndex() << std::endl;
}

MainViewWidget::~MainViewWidget()
{
    delete ui;
}

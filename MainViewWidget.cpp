#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"

#include <nix.hpp>

MainViewWidget::MainViewWidget(std::string& nix_file_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    ui->setupUi(this);

    // TODO hier weiter!
    static nix::File f = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);

}

MainViewWidget::~MainViewWidget()
{
    delete ui;
}

#include "DescriptionPanel.hpp"
#include "ui_DescriptionPanel.h"

DescriptionPanel::DescriptionPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptionPanel)
{
    ui->setupUi(this);
}

DescriptionPanel::~DescriptionPanel()
{
    delete ui;
}

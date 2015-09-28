#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"

MetaDataPanel::MetaDataPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);
}

MetaDataPanel::~MetaDataPanel()
{
    delete ui;
}

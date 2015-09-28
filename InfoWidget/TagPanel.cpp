#include "TagPanel.hpp"
#include "ui_TagPanel.h"

TagPanel::TagPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagPanel)
{
    ui->setupUi(this);
}

TagPanel::~TagPanel()
{
    delete ui;
}

#include "TagPanel.hpp"
#include "ui_TagPanel.h"

#include <iostream>

TagPanel::TagPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagPanel)
{
    ui->setupUi(this);
}

void TagPanel::update_tag_panel()
{
    std::cout << "lol?" << std::endl;
}

TagPanel::~TagPanel()
{
    delete ui;
}

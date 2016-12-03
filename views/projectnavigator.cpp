#include "projectnavigator.hpp"
#include "ui_projectnavigator.h"

ProjectNavigator::ProjectNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectNavigator)
{
    ui->setupUi(this);
}

ProjectNavigator::~ProjectNavigator()
{
    delete ui;
}

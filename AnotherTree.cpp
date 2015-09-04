#include "AnotherTree.hpp"
#include "ui_AnotherTree.h"

AnotherTree::AnotherTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnotherTree)
{
    ui->setupUi(this);
}

AnotherTree::~AnotherTree()
{
    delete ui;
}

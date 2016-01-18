#include "ColumnView.hpp"
#include "ui_ColumnView.h"

ColumnView::ColumnView(NixDataModel *_nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnView)
{
    ui->setupUi(this);
    nix_model = _nix_model;

    QColumnView* qcl = new QColumnView(this);
    qcl->setModel(nix_model);

    ui->verticalLayout->addWidget(qcl);
}

ColumnView::~ColumnView()
{
    delete ui;
}

#include "ColumnView.hpp"
#include "ui_ColumnView.h"

ColumnView::ColumnView(NixProxyModel *_nix_model, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnView)
{
    ui->setupUi(this);
    nix_proxy_model = _nix_model;

    qcv = new QColumnView(this);
    qcv->setModel(nix_proxy_model);

    ui->verticalLayout->addWidget(qcv);
}

QColumnView* ColumnView::get_column_view() {
    return qcv;
}

ColumnView::~ColumnView()
{
    delete ui;
}

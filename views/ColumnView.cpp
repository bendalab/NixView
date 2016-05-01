#include "ColumnView.hpp"
#include "ui_ColumnView.h"

ColumnView::ColumnView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnView)
{
    ui->setupUi(this);
    nix_proxy_model = nullptr;
    qcv = new QColumnView(this);
    ui->verticalLayout->addWidget(qcv);
}

ColumnView::ColumnView(QSortFilterProxyModel *_nix_proxy_model, QWidget *parent) :
    ColumnView(parent)
{
    set_proxy_model(_nix_proxy_model);
}

void ColumnView::set_proxy_model(QSortFilterProxyModel *proxy_model)
{
    nix_proxy_model  = proxy_model;
    qcv->setModel(nix_proxy_model);
}

QColumnView* ColumnView::get_column_view() {
    return qcv;
}

ColumnView::~ColumnView()
{
    delete ui;
}

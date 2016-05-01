#include "lazyloadview.h"
#include "ui_lazyloadview.h"

LazyLoadView::LazyLoadView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LazyLoadView)
{
    ui->setupUi(this);
}

LazyLoadView::~LazyLoadView()
{
    delete ui;
}


QTreeView* LazyLoadView::getTreeView() {
    return ui->treeView;
}

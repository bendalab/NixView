#include "InfoWidget.hpp"
#include "ui_InfoWidget.h"

InfoWidget::InfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoWidget)
{
    ui->setupUi(this);
    is_active = false;
}

void InfoWidget::update_info_widget(QTreeWidgetItem* item, int column)
{
    if (!is_active)
    {
        emit add_info_widget();
        is_active = true;
    }

    ui->test_label->setText(item->text(column));
}

InfoWidget::~InfoWidget()
{
    delete ui;
}

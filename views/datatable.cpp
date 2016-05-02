#include "datatable.h"
#include "ui_datatable.h"
#include "common/Common.hpp"
#include "MainViewWidget.hpp"
#include <vector>


DataTable::DataTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTable), model()
{
    ui->setupUi(this);
    ui->navigation_widget->setVisible(false);
    ui->back_btn->setEnabled(false);
    ui->next_btn->setEnabled(false);
}


DataTable::~DataTable()
{
    delete model;
    delete ui;
}


bool DataTable::can_draw(const QModelIndex qml) const {
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    if (item->nixType() == NixType::NIX_DATA_ARRAY) {
        nix::DataArray da = item->itemData().value<nix::DataArray>();
        return nix::data_type_is_numeric(da.dataType()) && da.dimensionCount() < 3;
    } else if (item->nixType() == NixType::NIX_FEAT) {
        nix::DataArray da = item->itemData().value<nix::Feature>().data();
        return nix::data_type_is_numeric(da.dataType()) && da.dimensionCount() < 3;
    } else {
        return false;
    }
}


void DataTable::set_entity(const QModelIndex qml) {
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());

    if (item->nixType() == NixType::NIX_DATA_ARRAY) {
        this->array = item->itemData().value<nix::DataArray>();
    } else if (item->nixType() == NixType::NIX_FEAT){
        this->array = item->itemData().value<nix::Feature>().data();
    } else {
        return;
    }

    if (array.dataExtent().size() > 2) {
            ui->navigation_widget->setVisible(true);
            ui->total_count_label->setText(QString::fromStdString(nix::util::numToStr(array.dataExtent()[2])));
            ui->current_page->setText(QVariant(1).toString());
            ui->current_page->setValidator(new QIntValidator(0, (int)array.dataExtent()[2]));
            ui->next_btn->setEnabled(true);
    }
    build_model();
    ui->description->setText(QString::fromStdString(EntityDescriptor::describe(array)));
}



void DataTable::next_page() {
    int curr_page = QVariant(ui->current_page->text()).toInt();
    curr_page++;
    ui->current_page->setText(QVariant(curr_page).toString());
    select_page();
}


void DataTable::previous_page() {
    int curr_page = QVariant(ui->current_page->text()).toInt();
    curr_page--;
    ui->current_page->setText(QVariant(curr_page).toString());
    select_page();
}


void DataTable::select_page() {
    int curr_page = QVariant(ui->current_page->text()).toInt();
    ui->next_btn->setEnabled(curr_page < array.dataExtent()[2]);
    ui->back_btn->setEnabled(curr_page > 1);
    build_model(curr_page - 1);
}


void DataTable::build_model(int page) {
    if (this->model != nullptr)
        delete model;

    model = new NixArrayTableModel(this);
    model->set_source(array, page);
    ui->table->setModel(model);
    ui->table->setSelectionMode(QAbstractItemView::ContiguousSelection);
}


QTableView* DataTable::get_table() {
    return ui->table;
}

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
    NixModelItem *item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0){
        nix::DataArray array = item->get_nix_entity<nix::DataArray>();
        return nix::data_type_is_numeric(array.dataType()) && array.dimensionCount() < 3;
    } else
        return false;
}


void DataTable::set_entity(const QModelIndex qml) {
    NixModelItem *item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0) {
        this->array = item->get_nix_entity<nix::DataArray>();
        if (array.dataExtent().size() > 2) {
            ui->navigation_widget->setVisible(true);
            ui->total_count_label->setText(QString::fromStdString(nix::util::numToStr(array.dataExtent()[2])));
            ui->current_page->setText(QVariant(1).toString());
            ui->current_page->setValidator(new QIntValidator(0, (int)array.dataExtent()[2]));
            ui->next_btn->setEnabled(true);
        }
        build_model();
        describe();
    }
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


void DataTable::describe() {
    ui->description->clear();
    EntityDescriptor descr = basic_description(array.name(), array.type(), array.definition() ? *array.definition() : "", array.id(),
                                               nix::util::timeToStr(array.createdAt()), nix::util::timeToStr(array.updatedAt()));

    ui->description->setText(QString::fromStdString(descr.toHtml()));
}


EntityDescriptor DataTable::basic_description(const std::string &name, const std::string &type, const std::string &description,
                                   const std::string &id, const std::string &created, const std::string &updated ) {
    EntityDescriptor descr(name, type, description, id, created, updated);
    return descr;
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

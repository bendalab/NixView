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
        build_model();
        describe();
    }
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


void DataTable::build_model() {
    if (this->model != nullptr)
        delete model;

    model = new NixArrayTableModel(this);
    model->set_source(array);
    ui->table->setModel(model);
    ui->table->setSelectionMode(QAbstractItemView::ContiguousSelection);
}


QTableView* DataTable::get_table() {
    return ui->table;
}

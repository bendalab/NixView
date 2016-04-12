#include "datatable.h"
#include "ui_datatable.h"
#include "common/Common.hpp"
#include "MainViewWidget.hpp"



DataTable::DataTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTable), model()
{
    ui->setupUi(this);
}

DataTable::~DataTable()
{
    delete ui;
}


bool DataTable::can_draw(QModelIndex qml) const {
    NixModelItem *item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0){
        nix::DataArray array = item->get_nix_entity<nix::DataArray>();
        return nix::data_type_is_numeric(array.dataType()) && array.dimensionCount() < 3;
    } else
        return false;
}


void DataTable::set_entity(QModelIndex qml) {
    NixModelItem *item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0) {
        this->array = item->get_nix_entity<nix::DataArray>();
        build_model();
    }
}


void DataTable::build_model() {
    if (this->model != nullptr)
        delete model;
    nix::NDSize shape = this->array.dataExtent();
    model = new QStandardItemModel(shape[0], shape.size() > 1 ? shape[1] : 1, this);
    if (shape.size() == 1) {
        std::vector<double> data;
        data.resize(shape[0], 0.0);
        array.getData(nix::DataType::Double, data.data(), shape, {0});
        for (size_t row = 0; row < data.size(); row ++) {
            QModelIndex index = model->index(row, 0, QModelIndex());
            model->setData(index, data[row]);
        }
    } else {

    }
}

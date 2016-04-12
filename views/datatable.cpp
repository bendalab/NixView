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
    }
}


QStringList DataTable::axis_labels(const nix::DataArray &array, size_t dim) const {
    QStringList labels;
    if (dim >= 1 && dim <= array.dimensionCount()) {
        nix::Dimension d = array.getDimension(dim);
        if (d.dimensionType() == nix::DimensionType::Set) {
            std::vector<std::string> ls = d.asSetDimension().labels();
            for (std::string l : ls)
                labels.push_back(QString::fromStdString(l));
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            if (d.asRangeDimension().alias())
                return labels;
            std::vector<double> axis_data = d.asRangeDimension().ticks();
            std::string unit = d.asRangeDimension().unit() ? *d.asRangeDimension().unit() : "";
            for (double num : axis_data)
                labels.push_back(QString::fromStdString(nix::util::numToStr(num) + " " + unit));
        } else if (d.dimensionType() == nix::DimensionType::Sample) {
            nix::NDSize shape = array.dataExtent();
            std::vector<double> axis_data = d.asSampledDimension().axis(shape[dim - 1]);
            std::string unit = d.asSampledDimension().unit() ? *d.asSampledDimension().unit() : "";
            for (double num : axis_data)
                labels.push_back(QString::fromStdString(nix::util::numToStr(num) + " " + unit));
        }
    }
    return labels;
}


void DataTable::build_model() {
    if (this->model != nullptr)
        delete model;
    nix::NDSize shape = this->array.dataExtent();
    model = new QStandardItemModel(shape[0], shape.size() > 1 ? shape[1] : 1, this);
    ui->table->setModel(model);
    if (shape.size() == 1) {
        std::vector<double> data;
        data.resize(shape[0], 0.0);
        array.getData(nix::DataType::Double, data.data(), shape, {0});
        for (size_t row = 0; row < data.size(); row ++) {
            QModelIndex index = model->index(row, 0, QModelIndex());
            model->setData(index, data[row]);
        }
        QStringList hlabels;
        hlabels.push_back(QString::fromStdString((array.label() ? *array.label() : "") + (array.unit() ? " [" + *array.unit() + "]" : "")));
        model->setHorizontalHeaderLabels(hlabels);
        QStringList vlabels = axis_labels(array, 1);
        if (vlabels.size() > 0)
            model->setVerticalHeaderLabels(axis_labels(array, 1));
    } else {
        std::cerr << "sorry, not that elaborate, yet!" << std::endl;
    }
}

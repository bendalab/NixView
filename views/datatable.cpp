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
    ui->table->setModel(model);ui->table->setSelectionMode(QAbstractItemView::ContiguousSelection);
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
    } else if (shape.size() == 2) {
        std::vector<double> data;
        nix::NDSize count(2, 1);
        count[0] = shape[0];
        nix::NDSize offset(2, 0);
        for (size_t col = 0; col < shape[1]; col++) {
            data.resize(shape[0], 0.0);
            offset[1] = col;
            array.getData(nix::DataType::Double, data.data(), count, offset);
            for (size_t row = 0; row < data.size(); row++){
                QModelIndex index = model->index(row, col, QModelIndex());
                model->setData(index, data[row]);
            }
        }
        QStringList vlabels = axis_labels(array, 1);
        QStringList hlabels = axis_labels(array, 2);
        if (hlabels.size() > 0)
            model->setHorizontalHeaderLabels(hlabels);
        if (vlabels.size() > 0)
            model->setVerticalHeaderLabels(vlabels);
       ui->table->setToolTip(QString::fromStdString((array.label() ? *array.label() : "") + (array.unit() ? " [" + *array.unit() + "]" : "")));

    } else if (shape.size() == 3) {
        std::cerr << "sorry, not that elaborate, yet!" << std::endl;
    }
}

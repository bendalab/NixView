#ifndef NIXVIEW_PLOTTER_H
#define NIXVIEW_PLOTTER_H

#include <QVariant>
#include <QSignalMapper>
#include <qcustomplot.h>
#include <QWidget>
#include <nix.hpp>

namespace Ui {
    class Plotter;
    class PlotterType;
}

enum class PlotterType : unsigned int {
    Line, Category, Image
};

class Plotter {

public:

    virtual void add_events(const QVector<double> &x_data, const QVector<double> &y_data, const QString &name, bool y_scale=true) = 0;
    virtual void add_segments(const QVector<double> &positions, const QVector<double> &extents, const QString &name) = 0;
    virtual void set_label(const std::string &label) = 0;
    virtual PlotterType plotter_type() const = 0;
    static bool get_data_array_axis(const nix::DataArray &array, QVector<double> &axis_data, QVector<QString> &axis_labels, nix::ndsize_t dim) {
        if (dim == 0 || dim > array.dimensionCount())
            return false;
        nix::Dimension d = array.getDimension(dim);
        if (d.dimensionType() == nix::DimensionType::Sample) {
            nix::SampledDimension sd = d.asSampledDimension();
            std::vector<double> ax = sd.axis(array.dataExtent()[dim - 1]);
            axis_data = QVector<double>::fromStdVector(ax);
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            nix::RangeDimension rd = d.asRangeDimension();
            std::vector<double> ax = rd.axis(array.dataExtent()[0]);
            axis_data = QVector<double>::fromStdVector(ax);
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            nix::SetDimension sd = d.asSetDimension();
            std::vector<std::string> labels = sd.labels();
            for (size_t i = 0; i < labels.size(); ++i) {
                axis_labels.push_back(QString::fromStdString(labels[i]));
                axis_data.push_back(static_cast<double>(i));
            }
            if (labels.size() == 0) {
                for (nix::ndsize_t i = 0; i < array.dataExtent()[dim - 1]; ++i) {
                    axis_labels.push_back(QString::fromStdString(nix::util::numToStr<int>(i)));
                    axis_data.push_back(static_cast<double>(i));
                }
            }
        } else {
            std::cerr << "Unsupported dimension type" << std::endl;
            //unsupported
        }
        return true;
    }

    static void data_array_to_qvector(const nix::DataArray &array, QVector<double> &xdata, QVector<double> &ydata,
                                      QVector<QString> &xticklabels, nix::ndsize_t dim_index) {
        nix::Dimension d = array.getDimension(dim_index);
        //FIXME this would probably not work for any other than the first dimension...
        if (d.dimensionType() == nix::DimensionType::Sample) {
            nix::SampledDimension dim = d.asSampledDimension();
            std::vector<double> ax = dim.axis(array.dataExtent()[0]);
            xdata = QVector<double>::fromStdVector(ax);

            std::vector<double> data(ax.size());
            array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
            ydata = QVector<double>::fromStdVector(data);
        } else if (d.dimensionType() == nix::DimensionType::Range) {
            nix::RangeDimension dim = d.asRangeDimension();
            std::vector<double> ax = dim.axis(array.dataExtent()[0]);
            xdata = QVector<double>::fromStdVector(ax);

            std::vector<double> data(ax.size());
            array.getData(nix::DataType::Double, data.data(), {ax.size()}, {0});
            ydata = QVector<double>::fromStdVector(data);
            if (dim.alias()) {
                ydata.fill(1.0);
            }
        } else if (d.dimensionType() == nix::DimensionType::Set) {
            std::vector<double> data;
            array.getData(data);
            ydata = QVector<double>::fromStdVector(data);

            nix::SetDimension dim = d.asSetDimension();
            std::vector<std::string> labels = dim.labels();
            for (size_t i = 0; i < labels.size(); ++i) {
                xticklabels.push_back(QString::fromStdString(labels[i]));
                xdata.push_back(static_cast<double>(i));
            }
            if (labels.size() == 0) {
                for (int i = 0; i < ydata.size(); ++i) {
                    xticklabels.push_back(QString::fromStdString(nix::util::numToStr<int>(i)));
                    xdata.push_back(static_cast<double>(i));
                }
            }
        } else {
            std::cerr << "unsupported dimension type" << std::endl;
        }
    }

    static void data_array_ax_labels(const nix::DataArray &array, QString &ylabel, QVector<QString> &labels){
        if (array.label())
            ylabel = QString::fromStdString(*array.label());
        if (array.unit())
            ylabel = ylabel + " [" + QString::fromStdString(*array.unit()) + "]";
        for (nix::ndsize_t i = 0; i < array.dimensionCount(); i++) {
            nix::Dimension d = array.getDimension(i + 1);
            std::string dim_label;
            if (d.dimensionType() == nix::DimensionType::Sample) {
                nix::SampledDimension dim = d.asSampledDimension();
                if (dim.label())
                    dim_label = *dim.label();
                if (dim.unit())
                    dim_label = dim_label + " [" + *dim.unit() + "]";
            } else if (d.dimensionType() == nix::DimensionType::Range) {
                nix::RangeDimension dim = d.asRangeDimension();
                if (dim.label())
                    dim_label = *dim.label();
                if (dim.unit())
                    dim_label = dim_label + " [" + *dim.unit() + "]";
            } else if (d.dimensionType() == nix::DimensionType::Set) {
                dim_label = "";
            } else {
                std::cerr << "unsupported dimension type" << std::endl;
            }
            labels.push_back(QString::fromStdString(dim_label));
        }
    }


    static bool check_plottable_dtype(const nix::DataArray &array) {
        bool plottable = true;
        plottable = plottable && array.dataType() != nix::DataType::Bool;
        plottable = plottable && array.dataType() != nix::DataType::String;
        plottable = plottable && array.dataType() != nix::DataType::Char;
        plottable = plottable && array.dataType() != nix::DataType::Opaque;
        plottable = plottable && array.dataType() != nix::DataType::Nothing;
        return plottable;
    }
};
#endif // NIXVIEW_PLOTTER_H

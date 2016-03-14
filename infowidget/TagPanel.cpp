#include "TagPanel.hpp"
#include "ui_TagPanel.h"
#include "common/Common.hpp"
#include <model/NixDataModel.hpp>
#include "model/NixModelItem.hpp"
#include "MainViewWidget.hpp"

#include <iostream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

TagPanel::TagPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagPanel)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

// TODO set all labels to "-" if empty item is emitted

void TagPanel::update_tag_panel(QModelIndex qml)
{
    NixDataModel *current_model = MainViewWidget::get_current_model();
    NixModelItem *model_item = static_cast<NixModelItem*>(current_model->itemFromIndex(qml));

    clear_tag_panel();
    if(strcmp(model_item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0)
    {
        nix::Tag tag = model_item->get_nix_entity<nix::Tag>();
        extract_tag_info(tag);
        current_qml = qml;
    }
    else if(strcmp(model_item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0)
    {
        nix::MultiTag mtag = model_item->get_nix_entity<nix::MultiTag>();
        extract_multitag_info(mtag);
        current_qml = qml;
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

std::string TagPanel::extract_tag_info(nix::Tag tag)
{
    std::stringstream ss;

    std::vector<double> pos = tag.position();
    std::ostringstream oss_pos;
    if(!pos.empty())
    {
        for (int i = 0; i < (int)pos.size(); ++i)
        {
            oss_pos << std::setprecision(5) << pos[i];
            if (i < (int)pos.size()-1)
                oss_pos << ", ";
        }
    }
    ui->tableWidget->insertRow(0);
    QTableWidgetItem* item_pos = new QTableWidgetItem(QString::fromStdString(oss_pos.str()));
    ui->tableWidget->setItem(0, 0, item_pos);

    std::vector<double> ext = tag.extent();
    std::ostringstream oss_ext;
    if(!ext.empty())
    {
        for (int i = 0; i < (int)ext.size(); ++i)
        {
            oss_ext << std::setprecision(5) << ext[i];
            if (i < (int)ext.size()-1)
                oss_ext << ", ";
        }
    }
    QTableWidgetItem* item_ext = new QTableWidgetItem(QString::fromStdString(oss_ext.str()));
    ui->tableWidget->setItem(0, 1, item_ext);

    std::vector<std::string> units = tag.units();
    QTableWidgetItem* item_unit = nullptr;
    if(!units.empty())
        item_unit = new QTableWidgetItem(QString::fromStdString(units[0]));
    ui->tableWidget->setItem(0, 2, item_unit);

    references = tag.references();
    fill_tree(ui->treeWidget_references, references);

    std::vector<nix::Feature> _features = tag.features();
    features.clear();
    for (auto i : _features)
        features.push_back(i.data());
    fill_tree(ui->treeWidget_features, features);

    return ss.str();
}

void TagPanel::extract_multitag_info(nix::MultiTag mtag)
{
    std::stringstream ss;

    nix::DataArray positions = mtag.positions();
    nix::DataArray extents = mtag.extents();

    nix::NDSize pos_size = positions.dataExtent();

    if (pos_size.size() == 1)  // 1-dimensional data
    {
        std::vector<std::string> units = mtag.units();
        std::vector<double> pos_array;
        std::vector<double> ext_array;
        if(extents)
            positions.getData(ext_array);
        positions.getData(pos_array);
        for (int i = 0; i < (int)pos_size[0]; i++)
        {
            std::ostringstream ss;
            ss << std::setprecision(5) << pos_array[i];
            std::string pos_str = ss.str();
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(pos_str));

            int new_row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(new_row);
            ui->tableWidget->setItem(new_row, 0, item);

            if(extents)
            {
                std::ostringstream ss;
                ss << std::setprecision(5) << ext_array[i];
                QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(ss.str()));
                ui->tableWidget->setItem(new_row, 1, item);
            }

            if(units.size() > 0)
            {
                QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(units[0]));
                ui->tableWidget->setItem(new_row, 2, item);
            }
        }
    }
    else // TODO if (size_pos.size() > 2)  // = not 1-dimensional data
    {
        int dim_1 = pos_size[0];
        int dim_2 = pos_size[1];

        // NOT WORKING
//        typedef boost::multi_array<double, 2> array_type;
//        array_type pos_array(boost::extents[dim_1][dim_2]);
//        positions.getData(pos_array);

        std::vector<std::string> units = mtag.units();
        double* pos_array = new double[dim_1 * dim_2];
        positions.getDataDirect(nix::DataType::Double, pos_array, pos_size, {0,0});
        double* ext_array = new double[dim_1 * dim_2];
        if (extents)
            extents.getDataDirect(nix::DataType::Double, ext_array, pos_size, {0,0});
        for (int i = 0; i < dim_1; ++i)
        {
            int new_row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(new_row);

            std::ostringstream ss_pos;
            std::ostringstream ss_ext;
            std::ostringstream ss_units;

            for (int j =  0; j < dim_2; ++j)
            {
                ss_pos << std::setprecision(5) << pos_array[i*dim_2 + j];
                if (j < dim_2 - 1)
                    ss_pos << ", ";

                if (extents)
                {
                    ss_ext << std::setprecision(5) << ext_array[i*dim_2 + j];
                    if (j < dim_2 - 1)
                        ss_ext << ", ";
                }

                if (units.size() > 0)
                {
                    ss_units << units[j];
                    if (j < dim_2 - 1)
                        ss_units << ", ";
                }

            }
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(ss_pos.str()));
            ui->tableWidget->setItem(new_row, 0, item);

            if (extents)
            {
                QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(ss_ext.str()));
                ui->tableWidget->setItem(new_row, 1, item);
            }

            if(units.size() > 0)
            {
                QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(ss_units.str()));
                ui->tableWidget->setItem(new_row, 2, item);
            }
        }


    }

    references = mtag.references();
    fill_tree(ui->treeWidget_references, references);

    std::vector<nix::Feature> _features = mtag.features();
    features.clear();
    for (auto i : _features)
        features.push_back(i.data());
    fill_tree(ui->treeWidget_features, features);
}

void TagPanel::fill_tree(QTreeWidget* tree, std::vector<nix::DataArray> ar)
{
    for (auto i : ar)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree, QStringList(QString::fromStdString(i.name())));
        item->setText(1, QString::fromStdString(i.type()));
        item->setText(2, QString::fromStdString("Data Array"));
        item->setText(3, QString::fromStdString(nix::data_type_to_string(i.dataType())));
        std::stringstream s;
        s << i.dataExtent();
        std::string shape = s.str();
        boost::algorithm::trim(shape);
        shape = shape.substr(7, shape.length()-1);
        item->setText(4, QString::fromStdString(shape));
    }
}

void TagPanel::clear_tag_panel()
{
    int table_size  = ui->tableWidget->rowCount();
    for (int i = 0; i < table_size; ++i)
        ui->tableWidget->removeRow(0);
    this->ui->treeWidget_references->clear();
    this->ui->treeWidget_features->clear();
}

// slots
void TagPanel::reference_item_requested(QTreeWidgetItem* item, int column)
{
    if (!item)
    {
        emit references_cleared();
        return;
    }
    for (auto i : references)
    {
        if(i.name() == item->text(0).toStdString())
            emit emit_reference(i);
    }
}

void TagPanel::feature_item_requested(QTreeWidgetItem* item, int column)
{
    if (!item)
    {
        emit features_cleared();
        return;
    }
    for (auto i : features)
    {
        if(i.name() == item->text(0).toStdString())
            emit emit_feature(i);
    }
}

void TagPanel::tag_item_requested(int current_row, int current_column, int previous_row, int previous_column)
{
    emit emit_tag(current_qml, current_row);
}

void TagPanel::currentItemChanged_reference_helper(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    reference_item_requested(current, 0);
}

void TagPanel::currentItemChanged_feature_helper(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    feature_item_requested(current, 0);
}

// getters
QTreeWidget* TagPanel::get_reference_tree()
{
    return ui->treeWidget_references;
}

QTreeWidget* TagPanel::get_feature_tree()
{
    return ui->treeWidget_features;
}

QTableWidget* TagPanel::get_tag_table()
{
    return ui->tableWidget;
}

// destructor
TagPanel::~TagPanel()
{
    delete ui;
}

#include "TagPanel.hpp"
#include "ui_TagPanel.h"
#include "common/Common.hpp"

#include <iostream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include <QtGui/QTableWidget>

TagPanel::TagPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagPanel)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void TagPanel::update_tag_panel(QVariant v)
{
    clear_tag_panel();
    if(v.canConvert<nix::Tag>())
    {
        nix::Tag tag = v.value<nix::Tag>();
        extract_tag_info(tag);
        current_tag = v;
    }
    else if(v.canConvert<nix::MultiTag>())
    {
        nix::MultiTag mtag = v.value<nix::MultiTag>();
        extract_multitag_info(mtag);
        current_tag = v;
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
    QTableWidgetItem* item_unit;
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
    nix::NDSize size_pos = positions.dataExtent();
    nix::DataArray extents = mtag.extents();
    std::vector<double> ext_array;
    if(extents)
        positions.getData(ext_array);
    std::vector<std::string> units = mtag.units();
    if (size_pos.size() == 1)
    {
        std::vector<double> pos_array;
        positions.getData(pos_array);
        for (int i = 0; i < (int)size_pos[0]; i++)
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
        item->setText(1, QString::fromStdString("Data Array"));
        item->setText(2, QString::fromStdString(nix::data_type_to_string(i.dataType())));
        std::stringstream s;
        s << i.dataExtent();
        std::string shape = s.str();
        boost::algorithm::trim(shape);
        shape = shape.substr(7, shape.length()-1);
        item->setText(3, QString::fromStdString(shape));
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
    for (auto i : references)
    {
        if(i.name() == item->text(0).toStdString())
            emit emit_reference(i);
    }
}

void TagPanel::feature_item_requested(QTreeWidgetItem* item, int column)
{
    for (auto i : features)
    {
        if(i.name() == item->text(0).toStdString())
            emit emit_feature(i);
    }
}

void TagPanel::tag_item_requested(int current_row, int current_column, int previous_row, int previous_column)
{
    emit emit_tag(current_tag, current_row);
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

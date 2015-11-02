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
}

void TagPanel::update_tag_panel(QVariant v)
{
    clear_tag_panel();
    if(v.canConvert<nix::Tag>())
    {
        nix::Tag tag = v.value<nix::Tag>();
        //ui->label_tag_info->setText(QString::fromStdString(extract_tag_info(tag)));
        extract_tag_info(tag);
    }
    else if(v.canConvert<nix::MultiTag>())
    {
        nix::MultiTag mtag = v.value<nix::MultiTag>();
        extract_multitag_info(mtag);
    }
}

std::string TagPanel::extract_tag_info(nix::Tag tag)
{
    std::stringstream ss;

    std::vector<double> pos = tag.position();
    std::ostringstream oss_pos;
    if(!pos.empty())
        std::copy(pos.begin(), pos.end()-1, std::ostream_iterator<double>(oss_pos, ", "));

    ss << "Position: " << oss_pos.str() << "\n";

    std::vector<double> ext = tag.extent();
    std::ostringstream oss_ext;
    if(!ext.empty())
        std::copy(ext.begin(), ext.end()-1, std::ostream_iterator<double>(oss_ext, ", "));
    ss << "Extend: " << oss_ext.str() << "\n";

    std::vector<std::string> units = tag.units();
    std::ostringstream oss_units;
    if(!units.empty())
        std::copy(units.begin(), units.end()-1, std::ostream_iterator<std::string>(oss_units, ", "));
    ss<< "Units: " << oss_units.str();

    references = tag.references();
    fill_tree(ui->treeWidget_references, references);

    std::vector<nix::Feature> _features = tag.features();
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
        ui->tableWidget->resizeColumnsToContents();
        ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    }
    else //if (size_pos.size() == 2)
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
    //this->ui->label_tag_info->setText("");
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
            emit item_found(i);
    }
}

void TagPanel::feature_item_requested(QTreeWidgetItem* item, int column)
{
    for (auto i : features)
    {
        if(i.name() == item->text(0).toStdString())
            emit item_found(i);
    }
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

// destructor
TagPanel::~TagPanel()
{
    delete ui;
}

#include "TagPanel.hpp"
#include "ui_TagPanel.h"
#include "common/Common.hpp"

#include <iostream>
#include <sstream>
#include <ostream>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>

TagPanel::TagPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TagPanel)
{
    ui->setupUi(this);
}

void TagPanel::update_tag_panel(QVariant v)
{

    if(v.canConvert<nix::Tag>())
    {
        nix::Tag tag = v.value<nix::Tag>();
        ui->labal_tag_info->setText(QString::fromStdString(extract_tag_info(tag)));
    }
    else if(v.canConvert<nix::MultiTag>())
    {
        nix::MultiTag mtag = v.value<nix::MultiTag>();
        ui->labal_tag_info->setText(QString::fromStdString(extract_multitag_info(mtag)));
    }
    else
    {
        clear_tag_panel();
        return;
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

std::string TagPanel::extract_multitag_info(nix::MultiTag mtag)
{
    std::stringstream ss;

    nix::DataArray positions = mtag.positions();
    nix::NDSize size_pos = positions.dataExtent();
    std::ostringstream oss_pos;
    std::cout << size_pos[0] << std::endl;
    if (size_pos.size() == 1)
    {
        std::vector<double> pos_array;
        positions.getData(pos_array);
        std::cout << "debug" << std::endl;
        for (int i = 0; i < (int)size_pos[0]; i++)
            oss_pos << "Position " << i+1 << ": [ " << pos_array[i] <<" ]" << std::endl;
    }
    else //if (size_pos.size() == 2)
    {
        double** pos_array = new double*[size_pos[1]];
        std::cout << "debug1" << std::endl;
        for (int i = 0; i < (int)size_pos[1]; i++)
            pos_array[i] = new double[size_pos[0]];
        std::cout << "debug2" << std::endl;
        positions.getData(**pos_array);
        std::cout << "debug3" << std::endl;
        for (int i = 0; i < (int)size_pos[0]; i++)
        {
            oss_pos << "Position " << i+1 << ": [ ";
            for (int j = 0; j < (int)size_pos[1]; j++)
                oss_pos << pos_array[i+j] << " ";
            oss_pos << "]" << std::endl;
        }
        std::cout << "debug4" << std::endl;
    }
    ss << oss_pos.str();

    nix::DataArray extents = mtag.extents();
    std::ostringstream oss_ext;
    if (extents)
    {
        nix::NDSize size_ext =  extents.dataExtent();
        if (size_ext.size() == 1)
        {
            double* ext_array = new double[size_ext[0]];
            positions.getData(*ext_array);
            for (int i = 0; i < (int)size_ext[0]; i++)
                oss_ext << "Extend " << i+1 << ": [" << ext_array[i] <<"]" << std::endl;
        }
        else if (size_ext.size() == 2)
        {
            std::vector<double> ext_array;
            positions.getData(ext_array);
            for (int i = 0; i < (int)size_ext[0]; i++)
            {
                oss_ext << "Extend " << i+1 << ": [ ";
                for (int j = 0; j < (int)size_ext[1]; j++)
                    oss_pos << ext_array[i+j] << " ";
                oss_ext << "]" << std::endl;
            }
        }
    }
    else
        oss_ext << "No Extents" << std::endl;
    ss << oss_ext.str();

    std::vector<std::string> units = mtag.units();
    std::ostringstream oss_units;
    if (!units.empty())
    {
        oss_units << "Units: ";
        std::copy(units.begin(), units.end()-1, std::ostream_iterator<std::string>(oss_units, ", "));
    }
    else
        oss_units << "No Units";
    ss << oss_units.str();

    references = mtag.references();
    fill_tree(ui->treeWidget_references, references);

    std::vector<nix::Feature> _features = mtag.features();
    for (auto i : _features)
        features.push_back(i.data());
    fill_tree(ui->treeWidget_features, features);

    return ss.str();
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
    this->ui->labal_tag_info->setText("");
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

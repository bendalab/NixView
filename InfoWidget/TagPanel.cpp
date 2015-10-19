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
    nix::NDSize size = positions.dataExtent();
    positions.getData(pos_array);
    std::ostringstream oss_pos;

    nix::DataArray extents = mtag.extents();
    std::ostringstream oss_ext;

    std::vector<std::string> units = mtag.units();
    std::ostringstream oss_units;

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

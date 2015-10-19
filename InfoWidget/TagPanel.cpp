#include "TagPanel.hpp"
#include "ui_TagPanel.h"
#include "common/Common.hpp"

#include <iostream>
#include <sstream>
#include <ostream>
#include <boost/algorithm/string.hpp>

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
        nix::MultiTag mt = v.value<nix::MultiTag>();
        std::stringstream ss;
        ss << "test ";
        QString info_string = QString::fromStdString(ss.str());
        ui->labal_tag_info->setText(info_string);
    }
    else
    {
        clear_tag_panel();
        return;
    }
}

std::string TagPanel::extract_tag_info(nix::Tag tag)
{
    std::vector<double> pos = tag.position();
    std::ostringstream oss_pos;
    if(!pos.empty())
        std::copy(pos.begin(), pos.end()-1, std::ostream_iterator<double>(oss_pos, ", "));

    std::stringstream ss;
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
    fill_reference_list();

    // TODO
    std::vector<nix::Feature>  features = tag.features();
    std::ostringstream oss_features;
    if(!features.empty())
    {
        for (auto i : features)
        {
            continue;
        }
//        std::copy(features.begin(), features.end()-1, std::ostream_iterator<nix::Feature>(oss_features, ", "));
    }

    return ss.str();
}

void TagPanel::fill_reference_list()
{
    for (auto i : references)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(i.name())));
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

TagPanel::~TagPanel()
{
    delete ui;
}

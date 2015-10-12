#include "TagPanel.hpp"
#include "ui_TagPanel.h"
#include "common/Common.hpp"

#include <iostream>
#include <sstream>
#include <ostream>

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

    // references, features,

    return ss.str();
}

void TagPanel::clear_tag_panel()
{
    this->ui->labal_tag_info->setText("");
}

TagPanel::~TagPanel()
{
    delete ui;
}

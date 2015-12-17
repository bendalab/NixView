#include "MetaDataPanel.hpp"
#include "ui_MetaDataPanel.h"
#include "common/Common.hpp"
#include <ostream>

MetaDataPanel::MetaDataPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetaDataPanel)
{
    ui->setupUi(this);
}

// TODO set all labels to "-" if empty item is emitted

void MetaDataPanel::update_metadata_panel(QVariant v)
{
    ui->treeWidget->clear();
    if(v.canConvert<nix::Section>())
        ui->metadata_label->setText("Metadata Entries");
    else if(v.canConvert<nix::Property>())
        ui->metadata_label->setText("Property Value");
    else
        ui->metadata_label->setText("Connected Metadata");

    nix::Section meta_section;
    nix::Property meta_property;
    // check if value of v is entity with metadata
    if(v.canConvert<nix::Block>())
    {
        nix::Block block = v.value<nix::Block>();
        meta_section = block.metadata();
    }
    else if(v.canConvert<nix::DataArray>())
    {
        nix::DataArray da = v.value<nix::DataArray>();
        meta_section = da.metadata();
    }
    else if(v.canConvert<nix::Source>())
    {
        nix::Source so = v.value<nix::Source>();
        meta_section = so.metadata();
    }
    else if(v.canConvert<nix::Tag>())
    {
        nix::Tag tag = v.value<nix::Tag>();
        meta_section = tag.metadata();
    }
    else if(v.canConvert<nix::MultiTag>())
    {
        nix::MultiTag mtag = v.value<nix::MultiTag>();
        meta_section = mtag.metadata();
    }
    else if(v.canConvert<nix::Section>())
        meta_section = v.value<nix::Section>();
    else if(v.canConvert<nix::Property>())
        meta_property = v.value<nix::Property>();
    else
    {
        clear_metadata_panel();
        return;
    }

    if(meta_section)
    {
        QTreeWidgetItem* branch = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(meta_section.name())));
        branch->setText(1, QString::fromStdString(meta_section.type()));

        for (nix::Section s : meta_section.sections())
        {
            QTreeWidgetItem* tree_item = new QTreeWidgetItem(branch, QStringList(QString::fromStdString(s.name())));
            tree_item->setText(1, QString::fromStdString(s.type()));
            add_children_to_item(tree_item, s);
        }
        add_properties_to_item(branch, meta_section);

        ui->treeWidget->expandItem(branch);
        ui->treeWidget->resizeColumnToContents(0);
    }
    else if (meta_property)
    {
        QTreeWidgetItem* prop = new QTreeWidgetItem(ui->treeWidget, QStringList(QString::fromStdString(meta_property.name())));
        prop->setText(2, QString::fromStdString("Metadata"));
        prop->setText(3, QString::fromStdString(nix::data_type_to_string(meta_property.dataType())));
        add_values_to_property(prop, meta_property);
    }
    else
        clear_metadata_panel();
}

void MetaDataPanel::add_children_to_item(QTreeWidgetItem* item, nix::Section section)
{
    for  (auto s : section.sections())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(s.name())));
        child_item->setText(1, QString::fromStdString(s.type()));
        add_children_to_item(child_item, s);
    }

    add_properties_to_item(item, section);
}

void MetaDataPanel::add_properties_to_item(QTreeWidgetItem* item, nix::Section section)
{
    for (nix::Property p : section.properties())
    {
        QTreeWidgetItem* child_item = new QTreeWidgetItem(item, QStringList(QString::fromStdString(p.name())));
        child_item->setText(2, QString::fromStdString("Metadata"));
        child_item->setText(3, QString::fromStdString(nix::data_type_to_string(p.dataType())));
        add_values_to_property(child_item, p);
    }
}

void MetaDataPanel::add_values_to_property(QTreeWidgetItem* item, nix::Property p)
{
    std::vector<nix::Value> values = p.values();
    std::ostringstream oss;
    oss << "(";
    for (int i = 0; i < (int)values.size(); ++i)
    {
        if (item->text(3) == QString("String")) {
            std::string value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else if (item->text(3) == QString("Bool")){
            bool value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else if (item->text(3) == QString("Int32")){
            int32_t value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else if (item->text(3) == QString("Int64")){
            int64_t value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else if (item->text(3) == QString("UInt64")){
            uint64_t value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else if (item->text(3) == QString("Double")){
            double value;
            values[i].get(value);
            oss << value << ", " << values[i].uncertainty;
        } else {
            oss << "NOT READABLE" << values[i].uncertainty;
        }
        oss << ")";

        if (i < (int)values.size()-1)
            oss << ", (";
    }
    item->setText(4, QString::fromStdString(oss.str()));
}

void MetaDataPanel::clear_metadata_panel()
{
    ui->treeWidget->clear();
}

void MetaDataPanel::resize_to_content(QModelIndex qml)
{
    for (int c = 0; c<ui->treeWidget->columnCount();c++)
        ui->treeWidget->resizeColumnToContents(c);
}

//getter
const QTreeWidget* MetaDataPanel::get_tree_widget()
{
    return ui->treeWidget;
}

MetaDataPanel::~MetaDataPanel()
{
    delete ui;
}

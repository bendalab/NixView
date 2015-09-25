#include "MainViewWidget.hpp"
#include "ui_MainViewWidget.h"
#include "RawTreeView.hpp"
#include "AnotherTree.hpp"


/**
* @brief Container for all widgets for data display.
* @param nix_file_path: path to opened nix file
*/
MainViewWidget::MainViewWidget(std::string& nix_file_path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainViewWidget)
{
    ui->setupUi(this);

    nix_file = nix::File::open(nix_file_path, nix::FileMode::ReadOnly);

    populate_data_stacked_widget();
    connect_widgets();
}

void MainViewWidget::populate_data_stacked_widget()
{
    rtv = new RawTreeView(nix_file);
    ui->data_stacked_Widget->addWidget(rtv);

    iw =  new InfoWidget();

    // for testing
    AnotherTree* aot  = new AnotherTree();
    ui->data_stacked_Widget->addWidget(aot);
    // =====
}

// slots
void MainViewWidget::data_combo_box_changed(int index) {
    ui->data_stacked_Widget->setCurrentIndex(index);
}

void MainViewWidget::activate_info_widget()
{
    ui->horizontalLayout->addWidget(iw);
}

void MainViewWidget::item_info_requested(QTreeWidgetItem* item, int column)
{
    std::vector<std::string> nix_path;
    QTreeWidgetItem* current_item = item;
    // don't do anything
    if (current_item->text(0) == QString("Metadata") || current_item->text(0)==QString("Data"))
        return;

    // get full path to nix_file root
    while (current_item->text(0)!=QString("Metadata") && current_item->text(0)!=QString("Data"))
    {
        nix_path.push_back(current_item->text(0).toStdString());
        current_item = current_item->parent();
    }

    // get data info
    if (current_item->text(0)==QString("Data"))
    {
        nix::Block block = nix_file.getBlock(nix_path.back());
        nix_path.pop_back();

        if(nix_path.size() == 0) //block info requested
        {
            emit item_info_found(block.id(), block.type(), block.name(), block.definition());
            return;
        }

        else if(nix_path.size() == 1) //data array/tag/multitag requested
        {
            if (item->text(1) == QString("Data Array"))
            {
                nix::DataArray da = block.getDataArray(item->text(0).toStdString());
                emit item_info_found(da.id(), da.type(), da.name(), da.definition());
            }
            else if (item->text(1) == QString("Tag"))
            {
                nix::Tag tag = block.getTag(item->text(0).toStdString());
                emit item_info_found(tag.id(), tag.type(), tag.name(), tag.definition());
            }
            else if (item->text(1) == QString("MultiTag"))
            {
                nix::MultiTag mtag = block.getMultiTag(item->text(0).toStdString());
                emit item_info_found(mtag.id(), mtag.type(), mtag.name(), mtag.definition());
            }
        }
    }
    else if (current_item->text(0) == QString("Metadata"))
    {
        nix::Section section = nix_file.getSection(nix_path.back());
        nix_path.pop_back();

        if(nix_path.size() == 0)
        {
            emit item_info_found(section.id(), section.type(), section.name(), section.definition());
            return;
        }

        while(nix_path.size() > 1)
        {
            section = section.getSection(nix_path.back());
            nix_path.pop_back();
        }

        if(!section.hasProperty(nix_path.back())) //section info requested
        {
            section = section.getSection(nix_path.back());
            emit item_info_found(section.id(), section.type(), section.name(), section.definition());
        }
        else
        {
            nix::Property property = section.getProperty(nix_path.back());
            emit item_info_found(property.id(), property.name(), property.definition());
        }
    }

}

// widget connection
void MainViewWidget::connect_widgets()
{
    // view mode combo box
    QObject::connect(ui->data_combo_box, SIGNAL(currentIndexChanged(int)), this, SLOT(data_combo_box_changed(int)));

    // activate info widget
    QObject::connect(iw, SIGNAL(add_info_widget()), this, SLOT(activate_info_widget()));

    // double click in overview
//    QObject::connect(rtv->get_tree_widget(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), iw, SLOT(update_info_widget(QTreeWidgetItem* ,int))); //test case
    QObject::connect(rtv->get_tree_widget(), SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(item_info_requested(QTreeWidgetItem*,int)));
    QObject::connect(this, SIGNAL(item_info_found(std::string,std::string,std::string,boost::optional<std::basic_string<char> >)),
                     iw, SLOT(update_info_widget(std::string, std::string, std::string, boost::optional<std::basic_string<char>>)));
    QObject::connect(this, SIGNAL(item_info_found(std::string,std::string,boost::optional<std::basic_string<char> >)),
                     iw, SLOT(update_info_widget(std::string, std::string, boost::optional<std::basic_string<char>>)));

    // overview expanded/collapsed
    QObject::connect(rtv->get_tree_widget(), SIGNAL(expanded(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
    QObject::connect(rtv->get_tree_widget(), SIGNAL(collapsed(QModelIndex)), rtv, SLOT(resize_to_content(QModelIndex)));
}

MainViewWidget::~MainViewWidget()
{
    delete ui;
}

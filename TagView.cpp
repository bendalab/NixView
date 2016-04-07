#include "TagView.hpp"
#include "ui_tagview.h"
#include "plotwidget.h"
#include "common/Common.hpp"
#include <QListWidgetItem>

TagView::TagView(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::TagView)
{
    ui->setupUi(this);
}

TagView::~TagView()
{
    delete ui;
}

void TagView::setEntity(QModelIndex qml) {
    this->item_qml = qml;
    this->item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    if (strcmp(this->item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0) {
        this->tag = item->get_nix_entity<nix::Tag>();
        EntityDescriptor descr = basic_description(tag.name(), tag.type(), tag.definition() ? *tag.definition() : "", tag.id(),
        nix::util::timeToStr(tag.createdAt()), nix::util::timeToStr(tag.updatedAt()));
        ui->entity_description->setText(QString::fromStdString(descr.toHtml()));
        fill_references();
        fill_features();
    }
}

EntityDescriptor TagView::basic_description(const std::string &name, const std::string &type, const std::string &description,
                                   const std::string &id, const std::string &created, const std::string &updated ) {
    EntityDescriptor descr(name, type, description, id, created, updated);
    return descr;
}


void TagView::fill_references() {
    ui->refernce_list->clear();
    for (nix::DataArray da : this->tag.references()) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(da.name() + " [" + da.type() + "]"));
        ui->refernce_list->addItem(item);
    }
}

void TagView::fill_features() {
    ui->feature_list->clear();
    for (nix::Feature f : this->tag.features()) {
        nix::DataArray da = f.data();
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(da.name() + " [" + da.type() + "]"));
        ui->feature_list->addItem(item);
    }

}

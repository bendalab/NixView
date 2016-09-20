#include "TagView.hpp"
#include "ui_TagView.h"
#include "common/Common.hpp"
#include <QListWidgetItem>
#include "plotter/plotter.h"
#include "plotter/lineplotter.h"
#include "plotter/plotwidget.h"

TagContainer::TagContainer(QVariant entity):entity(entity) {}

TagContainer::TagContainer(){}

std::vector<nix::Feature> TagContainer::features(){
    std::vector<nix::Feature> vec;
    if (entity.canConvert<nix::Tag>()) {
        vec = entity.value<nix::Tag>().features();
    } else if (entity.canConvert<nix::MultiTag>()) {
        vec = entity.value<nix::MultiTag>().features();
    }
    return vec;
}


std::vector<nix::DataArray> TagContainer::references() {
    std::vector<nix::DataArray> vec;
    if (entity.canConvert<nix::Tag>()) {
        vec = entity.value<nix::Tag>().references();
    } else if (entity.canConvert<nix::MultiTag>()) {
        vec = entity.value<nix::MultiTag>().references();
    }
    return vec;
}


std::string TagContainer::name() {
    std::string name;
    if (entity.canConvert<nix::Tag>()) {
        name = entity.value<nix::Tag>().name();
    } else if (entity.canConvert<nix::MultiTag>()) {
        name = entity.value<nix::MultiTag>().name();
    }
    return name;
}


std::string TagContainer::type() {
    std::string type;
    if (entity.canConvert<nix::Tag>()) {
        type = entity.value<nix::Tag>().type();
    } else if (entity.canConvert<nix::MultiTag>()) {
        type = entity.value<nix::MultiTag>().type();
    }
    return type;
}


std::string TagContainer::description() {
    std::string description;
    if (entity.canConvert<nix::Tag>()) {
        description = EntityDescriptor::describe(entity.value<nix::Tag>());
    } else if (entity.canConvert<nix::MultiTag>()) {
        description = EntityDescriptor::describe(entity.value<nix::MultiTag>());
    }
    return description;
}


QVariant TagContainer::getEntity() {
    return this->entity;
}


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
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    clear();
    if (item->nixType() == NixType::NIX_TAG) {
        this->tag = TagContainer(QVariant::fromValue(item->itemData().value<nix::Tag>()));
    } else if (item->nixType() == NixType::NIX_MTAG) {
        this->tag = TagContainer(QVariant::fromValue(item->itemData().value<nix::MultiTag>()));
    }
    ui->tagLabel->setText(QString::fromStdString(tag.name() + " - " + tag.type()));
    ui->tagLabel->setToolTip(tag.description().c_str());
    fill_references();
    fill_features();
}


void TagView::clear() {
    ui->tagLabel->setText("");
    clear_references();
    clear_features();
}


void TagView::clear_references() {
    ui->referencesCombo->clear();
    reference_map.clear();
    for (int i = ui->referenceStack->count() -1; i == 0; i--) {
        QWidget *w = ui->referenceStack->widget(i);
        ui->referenceStack->removeWidget(w);
        delete w;
    }
    QWidget *w = new QWidget();
    w->setLayout(new QVBoxLayout());
    ui->referenceStack->addWidget(w);
    ui->referenceStack->setCurrentIndex(0);
}


void TagView::clear_features() {
    ui->featuresCombo->clear();
    feature_map.clear();
    for (int i = ui->featureStack->count() -1; i == 0; i--) {
        QWidget *w = ui->featureStack->widget(i);
        ui->featureStack->removeWidget(w);
        delete w;
    }
    QWidget *w = new QWidget();
    w->setLayout(new QVBoxLayout());
    ui->featureStack->addWidget(w);
    ui->featureStack->setCurrentIndex(0);
}


void TagView::fill_references() {
    clear_references();
    for (nix::DataArray da : tag.references()) {
        ui->referencesCombo->addItem(QString::fromStdString(da.name() + " [" + da.type() + "]"), QVariant::fromValue(da));
    }
    if (ui->referencesCombo->count() > 0)
        ui->referencesCombo->setCurrentIndex(0);
}


void TagView::fill_features() {
    clear_features();
    for (nix::Feature f : this->tag.features()) {
        nix::DataArray da = f.data();
        ui->featuresCombo->addItem(QString::fromStdString(da.name() + " [" + da.type() + "]"), QVariant::fromValue(f));
    }
    if (ui->featuresCombo->count() > 0)
        ui->featuresCombo->setCurrentIndex(0);
}


void TagView::reference_selected(int i) {
    if (reference_map.size() == 0) {
        QWidget *w = ui->referenceStack->widget(0);
        ui->referenceStack->removeWidget(w);
        delete w;
    }
    if (reference_map.contains(i)) {
        ui->referenceStack->setCurrentIndex(reference_map[i]);
    } else if(i >= 0) {
        QWidget *w = new QWidget();
        w->setLayout(new QVBoxLayout());
        PlotWidget *pw = new PlotWidget();
        w->layout()->addWidget(pw);
        QVariant var = this->tag.getEntity();
        if (var.canConvert<nix::Tag>())
            pw->process(var.value<nix::Tag>(), i);
        else if (var.canConvert<nix::MultiTag>())
            pw->process(var.value<nix::MultiTag>(), i);
        ui->referenceStack->addWidget(w);
        reference_map[i] = ui->referenceStack->count() - 1;
        ui->referenceStack->setCurrentIndex(reference_map[i]);
    }
}


void TagView::feature_selected(int i) {
    if (feature_map.size() == 0) {
        QWidget *w = ui->featureStack->widget(0);
        ui->featureStack->removeWidget(w);
        delete w;
    }
    if (feature_map.contains(i)) {
        ui->featureStack->setCurrentIndex(feature_map[i]);
    } else if(i >= 0) {
        QWidget *w = new QWidget();
        w->setLayout(new QVBoxLayout());
        PlotWidget *pw = new PlotWidget();
        w->layout()->addWidget(pw);
        QVariant var = this->tag.getEntity();
        if (var.canConvert<nix::Tag>())
            pw->process(this->tag.features()[i], var.value<nix::Tag>());
        else if (var.canConvert<nix::MultiTag>())
            pw->process(this->tag.features()[i], var.value<nix::MultiTag>());
        ui->featureStack->addWidget(w);
        feature_map[i] = ui->featureStack->count() - 1;
        ui->featureStack->setCurrentIndex(feature_map[i]);
    }
}


void TagView::show_tag_info() {
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(this->tag.description().c_str());
    msgBox.exec();
}

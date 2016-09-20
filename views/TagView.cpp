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
    // QObject::connect(ui->reference_list, SIGNAL(itemSelectionChanged()), this, SLOT(reference_selected()));
    // QObject::connect(ui->feature_list, SIGNAL(itemSelectionChanged()), this, SLOT(feature_selected()));
}

TagView::~TagView()
{
    delete ui;
}

void TagView::setEntity(QModelIndex qml) {
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    clear();
    if (item->nixType() == NixType::NIX_TAG) {
        this->tag = item->itemData().value<nix::Tag>();
        ui->tagLabel->setText(QString::fromStdString(tag.name() + " - " + tag.type()));
        ui->tagLabel->setToolTip(EntityDescriptor::describe(tag).c_str());
        //ui->entity_description->setText(QString::fromStdString(EntityDescriptor::describe(tag)));
        fill_references();
        fill_features();
    }
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
}


void TagView::clear_features() {
    ui->featuresCombo->clear();
    feature_map.clear();
    for (int i = ui->featureStack->count() -1; i == 0; i--) {
        QWidget *w = ui->featureStack->widget(i);
        ui->featureStack->removeWidget(w);
        delete w;
    }
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
    ui->featuresCombo->clear();
    for (nix::Feature f : this->tag.features()) {
        nix::DataArray da = f.data();
        ui->featuresCombo->addItem(QString::fromStdString(da.name() + " [" + da.type() + "]"), QVariant::fromValue(da));
    }
    if (ui->featuresCombo->count() > 0)
        ui->featuresCombo->setCurrentIndex(0);
}


void TagView::reference_selected(int i) {
    if (reference_map.contains(i)) {
        ui->referenceStack->setCurrentIndex(reference_map[i]);
    } else if(i >= 0) {
        QWidget *w = new QWidget();
        w->setLayout(new QVBoxLayout());
        PlotWidget *pw = new PlotWidget();
        w->layout()->addWidget(pw);
        pw->process(this->tag, i);
        //Plotter *plot = pw->plot();
        ui->referenceStack->addWidget(w);
        reference_map[i] = ui->referenceStack->count() - 1;
        ui->referenceStack->setCurrentIndex(reference_map[i]);
    }
}


void TagView::feature_selected(int i) {
    if (feature_map.contains(i)) {
        ui->featureStack->setCurrentIndex(feature_map[i]);
    } else if(i >= 0) {
        QWidget *w = new QWidget();
        w->setLayout(new QVBoxLayout());
        PlotWidget *pw = new PlotWidget();
        w->layout()->addWidget(pw);
        pw->process(this->tag.features()[i], this->tag);
        ui->featureStack->addWidget(w);
        feature_map[i] = ui->featureStack->count() - 1;
        ui->featureStack->setCurrentIndex(feature_map[i]);
    }
}


void TagView::header_clicked() {
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(EntityDescriptor::describe(this->tag).c_str());
    msgBox.exec();
}

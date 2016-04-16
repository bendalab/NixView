#include "TagView.hpp"
#include "ui_TagView.h"
#include "plotter/plotwidget.h"
#include "common/Common.hpp"
#include <QListWidgetItem>
#include "plotter/plotter.h"
#include "plotter/lineplotter.h"

TagView::TagView(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::TagView)
{
    ui->setupUi(this);
    QObject::connect(ui->reference_list, SIGNAL(itemSelectionChanged()), this, SLOT(reference_selected()));
    QObject::connect(ui->feature_list, SIGNAL(itemSelectionChanged()), this, SLOT(feature_selected()));
}

TagView::~TagView()
{
    delete ui;
}

void TagView::setEntity(QModelIndex qml) {
    NixModelItem *item = MainViewWidget::get_current_model()->get_item_from_qml(qml);
    clear();
    if (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0) {
        this->tag = item->get_nix_entity<nix::Tag>();
        EntityDescriptor descr = basic_description(tag.name(), tag.type(), tag.definition() ? *tag.definition() : "", tag.id(),
        nix::util::timeToStr(tag.createdAt()), nix::util::timeToStr(tag.updatedAt()));
        ui->entity_description->setText(QString::fromStdString(descr.toHtml()));
        fill_references();
        fill_features();
    }
}


void TagView::clear() {
    ui->reference_list->clear();
    ui->entity_description->clear();
    ui->feature_list->clear();
    clear_plot_widget(ui->reference_plot);
    clear_plot_widget(ui->feature_plot);
}


void TagView::clear_plot_widget(QWidget *plot_widget) { //TODO this does not seem to work properly (linux)
    if (!plot_widget->layout()->isEmpty()) {
        QLayoutItem *item = plot_widget->layout()->itemAt(0);
        if (item->widget()) {
            plot_widget->layout()->removeItem(item);
            delete item;
            plot_widget->update();
            plot_widget->repaint();
        }
    }
}


EntityDescriptor TagView::basic_description(const std::string &name, const std::string &type, const std::string &description,
                                   const std::string &id, const std::string &created, const std::string &updated ) {
    EntityDescriptor descr(name, type, description, id, created, updated);
    return descr;
}


void TagView::fill_references() {
    ui->reference_list->clear();
    for (nix::DataArray da : this->tag.references()) {
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(da.name() + " [" + da.type() + "]"));
        ui->reference_list->addItem(item);
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


void TagView::reference_selected() {
   QList<QListWidgetItem*> items = ui->reference_list->selectedItems();
   if (items.size() > 0) {
       int index = ui->reference_list->row(items[0]);
       nix::DataArray da = tag.getReference(index);
       plot_data(ui->reference_plot, da);
    }
}


void TagView::feature_selected() {
    QList<QListWidgetItem*> items = ui->feature_list->selectedItems();
    if (items.size() > 0) {
        int index = ui->feature_list->row(items[0]);
        nix::DataArray da = tag.getFeature(index).data();
        plot_data(ui->feature_plot, da);
    }
}


void TagView::plot_data(QWidget *plot_widget, const nix::DataArray &array) {
        clear_plot_widget(plot_widget);
        if (Plotter::check_plottable_dtype(array)) {
            PlotterType type = Plotter::suggested_plotter(array);
            if (type == PlotterType::Unsupported || type == PlotterType::Image) {
                std::cerr << "unsupported plotter" << std::endl;
                // TODO show error message
            } else if (type == PlotterType::Line){
                LinePlotter *lp = new LinePlotter(this);
                lp->draw(array);
                plot_widget->layout()->addWidget(lp);
            }
        }

}

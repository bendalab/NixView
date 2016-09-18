#include "TagView.hpp"
#include "ui_TagView.h"
#include "common/Common.hpp"
#include <QListWidgetItem>
#include "plotter/plotter.h"
#include "plotter/lineplotter.h"
#include "plotter/plotwidget.h"

TagView::TagView(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::TagView)
{
    ui->setupUi(this);
    // QObject::connect(ui->reference_list, SIGNAL(itemSelectionChanged()), this, SLOT(reference_selected()));
    QObject::connect(ui->feature_list, SIGNAL(itemSelectionChanged()), this, SLOT(feature_selected()));
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
        ui->tagLabel->setText(QString::fromStdString(tag.name() + " - " + tag.type() + "\t" + "[...]"));
        ui->tagLabel->setToolTip(EntityDescriptor::describe(tag).c_str());
        //ui->entity_description->setText(QString::fromStdString(EntityDescriptor::describe(tag)));
        fill_references();
        fill_features();
    }
}


void TagView::clear() {
    ui->tagLabel->setText("");
    ui->feature_list->clear();
    clear_references();
    clear_features();
}


void TagView::clear_references() {
    for (int i = ui->referenceTabs->count(); i >= 0; i--)
        ui->referenceTabs->removeTab(i);
}


void TagView::clear_features() {
    if (!ui->feature_plot->layout()->isEmpty()) {
        QLayoutItem *item = ui->feature_plot->layout()->itemAt(0);
        if (item->widget()) {
            ui->feature_plot->layout()->removeItem(item);
            delete item;
            ui->feature_plot->update();
            ui->feature_plot->repaint();
        }
    }
}


void TagView::clear_plot_widget(PlotWidget *plot_widget) { //TODO this does not seem to work properly (linux)
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


void TagView::fill_references() {
    clear_references();
    for (nix::ndsize_t i = 0; i < this->tag.referenceCount(); i++) {
        QWidget *w = new QWidget();
        w->setLayout(new QVBoxLayout());
        PlotWidget *pw = new PlotWidget();
        w->layout()->addWidget(pw);
        int index = ui->referenceTabs->addTab(w, tag.references()[i].name().c_str());
        ui->referenceTabs->setTabToolTip(index, QString::fromStdString(EntityDescriptor::describe(tag.references()[i])));
        pw->process(this->tag, i);
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
 /*  QList<QListWidgetItem*> items = ui->reference_list->selectedItems();
   if (items.size() > 0) {
       int index = ui->reference_list->row(items[0]);
       nix::DataArray da = tag.getReference(index);
       plot_data(ui->reference_plot, da);
    }
    */
}


void TagView::feature_selected() {
    QList<QListWidgetItem*> items = ui->feature_list->selectedItems();
    if (items.size() > 0) {
        int index = ui->feature_list->row(items[0]);
        nix::DataArray da = tag.getFeature(index).data();

        //plot_data(static_cast<PlotWidget*>(ui->feature_plot), da);
    }
}


void TagView::plot_data(PlotWidget *plot_widget, const nix::DataArray &array) {
    plot_widget->clear();
    if (Plotter::check_plottable_dtype(array)) {
        plot_widget->process(array);
        /*
        PlotterType type = Plotter::suggested_plotter(array);
        if (type == PlotterType::Unsupported || type == PlotterType::Image) {
            std::cerr << "unsupported plotter" << std::endl;
            // TODO show error message
        } else if (type == PlotterType::Line){
            LinePlotter *lp = new LinePlotter(this);
            lp->draw(array);
            plot_widget->setPlotter(lp);
        }
        */
    }
}


void TagView::header_clicked() {
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::FramelessWindowHint);
    msgBox.setText(EntityDescriptor::describe(this->tag).c_str());
    msgBox.exec();
}

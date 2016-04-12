#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "aboutdialog.h"
#include "plotdialog.h"
#include "views/RawTreeView.hpp"
#include "nix.hpp"
#include "common/Common.hpp"
#include "model/NixDataModel.hpp"
#include "model/NixModelItem.hpp"
#include "tabledialog.hpp"


MainWindow::MainWindow(QWidget *parent, QApplication *app) : QMainWindow(parent),
    ui(new Ui::MainWindow) {

    QCoreApplication::setOrganizationName("g-node");
    QCoreApplication::setApplicationName("nixview");

    ui->setupUi(this);
    file_label = new QLabel(this);
    file_progress = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(new QLabel("File: ", this));
    ui->statusBar->addPermanentWidget(file_label, 1);
    ui->statusBar->addPermanentWidget(file_progress, 10);
    file_progress->setVisible(false);
    QObject::connect(app, SIGNAL(invalid_file_error()), this, SLOT(invalid_file_error()));
    ow = new OptionsWidget();
    connect_widgets();
}

void MainWindow::connect_widgets() {
    QObject::connect(this, SIGNAL(emit_view_change(int)), ui->main_view, SLOT(set_view(int)));
    QObject::connect(ui->main_view, SIGNAL(emit_current_qml(QModelIndex)), this, SLOT(item_selected(QModelIndex)));
    QObject::connect(ui->main_view, SIGNAL(emit_model_update(NixDataModel*)), this, SLOT(nix_model_update(NixDataModel*)));
    QObject::connect(ui->main_view, SIGNAL(emit_current_qml(QModelIndex)), ui->info_view, SLOT(update_info_widget(QModelIndex)));
    QObject::connect(ow->tree_view_options, SIGNAL(emit_rtv_column_display_changed()), ui->main_view->get_rtv(), SLOT(hide_columns()));
    QObject::connect(ui->main_view, SIGNAL(scan_progress_update()), this, SLOT(file_scan_progress()));
}

MainWindow::~MainWindow() {
    delete ui;
}

// slots
void MainWindow::on_actionTree_triggered()
{
    emit emit_view_change(VIEW_TREE);
}

void MainWindow::on_actionColumn_triggered()
{
    emit emit_view_change(VIEW_COLUMN);
}

void MainWindow::on_actionProperties_triggered()
{
    ow->show();
}


void MainWindow::nix_model_update(NixDataModel *model) {
    ui->info_view->setDataModel(model);
}


void MainWindow::item_selected(QModelIndex qml) {
    selected_qml = qml;
    ui->actionPlot->setEnabled(false);
    ui->actionTable->setEnabled(false);

    NixModelItem *item = ui->main_view->get_current_model()->get_item_from_qml(qml);

    if(strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_DATAARRAY) == 0) {
        ui->actionTable->setEnabled(true);
        ui->actionPlot->setEnabled(true);
    } else if ((strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_TAG) == 0) |
                (strcmp(item->get_nix_qvariant_type().c_str(), NIX_STRING_MULTITAG) == 0)) {
        ui->actionPlot->setEnabled(true);
    }
}


void MainWindow::show_about() {
    AboutDialog d(this);
    d.exec();
}


void MainWindow::show_plot() {
    PlotDialog d(this);
    d.set_entity(selected_qml);
    d.exec();
}


void MainWindow::show_table() {
    TableDialog d(this);
    d.set_entity(selected_qml);
    d.exec();
}

void MainWindow::file_scan_progress()
{
    file_progress->setValue(ui->main_view->get_scan_progress());
    QCoreApplication::processEvents();
}


void MainWindow::invalid_file_error() {
    file_label->setText("");
    file_progress->setVisible(false);
    QMessageBox::information(this, "Invalid file", "Nix library reports an error. The selected file is not a valid nix file.");
}


void MainWindow::open_file() {
    QFileDialog fd(this);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setNameFilter(tr("NIX File (*.nix *.h5)"));
    fd.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (fd.exec())
        fileNames = fd.selectedFiles();
    if (fileNames.size() == 0)
        return;

    std::string file_path = fileNames.front().toStdString();
    file_label->setText(file_path.c_str());
    file_progress->setVisible(true);

    ui->main_view->set_nix_file(file_path);
    file_progress->setVisible(false);
}

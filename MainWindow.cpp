#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/plotdialog.h"
#include "views/RawTreeView.hpp"
#include "nix.hpp"
#include "common/Common.hpp"
#include "model/nixtreemodel.h"
#include "model/nixtreemodelitem.h"
#include "dialogs/tabledialog.hpp"
#include <QSettings>
#include "utils/utils.hpp"


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
    ui->recent_file_list->setAttribute(Qt::WA_MacShowFocusRect, false);
    connect_widgets();
    get_recent_files();
}


void MainWindow::connect_widgets() {
    QObject::connect(ui->actionCloseFile, SIGNAL(triggered()), this, SLOT(close_file()));
    QObject::connect(this, SIGNAL(emit_view_change(int)), ui->main_view, SLOT(set_view(int)));
    QObject::connect(ui->main_view, SIGNAL(emit_current_qml(QModelIndex)), this, SLOT(item_selected(QModelIndex)));
    QObject::connect(ui->main_view, SIGNAL(emit_model_update(NixTreeModel*)), this, SLOT(nix_model_update(NixTreeModel*)));
    QObject::connect(ui->main_view, SIGNAL(emit_current_qml(QModelIndex)), ui->info_view, SLOT(update_info_widget(QModelIndex)));
    QObject::connect(ui->main_view, SIGNAL(scan_progress_update()), this, SLOT(file_scan_progress()));
    QObject::connect(ui->menu_open_recent, SIGNAL(triggered(QAction*)), this, SLOT(open_recent_file(QAction*)));
    QObject::connect(ui->actionFind, SIGNAL(triggered()), ui->main_view, SLOT(toggle_find()));
}


void MainWindow::get_recent_files() {
    QSettings *settings = new QSettings();
    settings->beginGroup(RECENT_FILES_GROUP);
    QStringList keys = settings->childKeys();
    recent_files.clear();
    for (QString k: keys) {
        recent_files.push_back(settings->value(k).toString());
    }
    nixview::util::remove_duplicates(recent_files);
    populate_recent_file_menu();
    settings->endGroup();
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


void MainWindow::nix_model_update(NixTreeModel *model) {
    ui->info_view->setDataModel(model);
}


void MainWindow::item_selected(QModelIndex qml) {
    selected_qml = qml;
    ui->actionPlot->setEnabled(false);
    ui->actionTable->setEnabled(false);
    NixTreeModelItem *item = static_cast<NixTreeModelItem*>(qml.internalPointer());
    NixType type = item->nixType();
    if(type == NixType::NIX_DATA_ARRAY | type == NixType::NIX_FEAT) {
        ui->actionTable->setEnabled(true);
        ui->actionPlot->setEnabled(true);
    } else if (type == NixType::NIX_TAG | type == NixType::NIX_MTAG) {
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
    read_nix_file(fileNames.front());
}


void MainWindow::close_file() {
    ui->stackedWidget->setCurrentIndex(1);
    ui->main_view->clear();
    ui->actionCloseFile->setEnabled(false);
    ui->actionPlot->setEnabled(false);
    ui->actionTable->setEnabled(false);
}

void MainWindow::read_nix_file(QString filename) {
    std::string file_path = filename.toStdString();
    file_label->setText(file_path.c_str());
    file_progress->setVisible(true);

    ui->main_view->set_nix_file(file_path);
    file_progress->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->actionCloseFile->setEnabled(true);
}


void MainWindow::populate_recent_file_menu() {
    ui->recent_file_list->clear();
    QList<QAction*> actions = ui->menu_open_recent->actions();
    for (QAction* a : actions) {
        ui->menu_open_recent->removeAction(a);
        delete a;
    }
    for (QString s : recent_files) {
        ui->menu_open_recent->addAction(s);
        QListWidgetItem *item = new QListWidgetItem(s);
        item->setToolTip(s);
        ui->recent_file_list->addItem(item);
    }
    ui->menu_open_recent->setEnabled(ui->menu_open_recent->actions().count() != 0);    
}


void MainWindow::recent_file_update(QStringList files) {
    this->recent_files = files;
    populate_recent_file_menu();
}


void MainWindow::open_recent_file(QAction *a) {
    read_nix_file(a->text());
}

void MainWindow::recent_file_selected(QListWidgetItem *item) {
    read_nix_file(item->text());
}

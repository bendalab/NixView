#include "TreeViewOptions.hpp"
#include "ui_TreeViewOptions.h"
#include "common/Common.hpp"
#include "views/RawTreeView.hpp"
#include "model/nixtreemodelitem.h"


TreeViewOptions::TreeViewOptions(QString role, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TreeViewOptions)
{
    this->role = role;
    ui->setupUi(this);
    this->settings = new QSettings;
    settings->beginGroup(this->role);
    create_checkboxes();
}


void TreeViewOptions::create_checkboxes(){
    QVBoxLayout * layout = new QVBoxLayout;
    for (QString col : NixTreeModelItem::columns) {
        QCheckBox *box = new QCheckBox(col);
        box->setChecked(settings->value(col, QVariant(true)).toBool());
        settings->setValue(col, QVariant(box->isChecked()));
        layout->addWidget(box);
        QObject::connect(box, SIGNAL(clicked(bool)), this, SLOT(column_state_changed()));
        boxes[box] = box->isChecked();
    }
    ui->check_boxes->setLayout(layout);
}


void TreeViewOptions::column_state_changed() {
    for (QCheckBox* b : boxes.keys()) {
        if (b->isChecked() != boxes.value(b)) {
           boxes[b] = b->isChecked();
           settings->setValue(b->text(), QVariant(b->isChecked()));
           emit column_change(this->role, b->text(), b->isChecked());
        }
    }
}


TreeViewOptions::~TreeViewOptions() {
    settings->endGroup();
    delete settings;
    delete ui;
}

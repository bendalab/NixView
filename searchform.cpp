#include "searchform.h"
#include "ui_searchform.h"

SearchForm::SearchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);
}

SearchForm::~SearchForm()
{
    delete ui;
}


void SearchForm::setNixFile(const nix::File &f) {
    this->nix_file = f;
}



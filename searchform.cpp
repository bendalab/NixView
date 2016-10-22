#include "searchform.h"
#include "ui_searchform.h"
#include <iostream>
#include "common/Common.hpp"


SearchForm::SearchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchForm) {
    ui->setupUi(this);
    QStringList filter_expressions = {FILTER_EXP_DATAARRAY, FILTER_EXP_BLOCK,
                                      FILTER_EXP_TAG, FILTER_EXP_MULTITAG, FILTER_EXP_GROUP,
                                      FILTER_EXP_SOURCE, FILTER_EXP_METADATA };
    ui->typeComboBox->addItems(filter_expressions);
    ui->fieldComboBox->addItems({"name", "type", "id", "definition"});
}


SearchForm::~SearchForm() {
    delete ui;
}


void SearchForm::go() {
    this->results.clear();
    if (this->nix_file != nix::none) {
        if (ui->termEdit->text().size() == 0) {
            return;
        }
        std::cerr << "going to search\n";
        switch (ui->typeComboBox->currentIndex()) {
            case 0:
                std::cerr << "in dataArrays" << std::endl;
                if (ui->typeComboBox->currentIndex() == 0) {
                    for (nix::Block b : this->nix_file.blocks()){
                        std::vector<nix::DataArray> arrays = b.dataArrays(nix::util::NameFilter<nix::DataArray>(ui->termEdit->text().toStdString()));
                        for (nix::DataArray da : arrays) {
                            this->results.push_back(QVariant::fromValue(da));
                        }
                    }
                } else if (ui->typeComboBox->currentIndex() == 1) {

                }
                break;
            default:
                break;
        }
        emit SearchForm::newResults(this->results);
    }
}


void SearchForm::setNixFile(const nix::File &f) {
    this->nix_file = f;
}



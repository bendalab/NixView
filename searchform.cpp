#include "searchform.h"
#include "ui_searchform.h"
#include <iostream>
#include "common/Common.hpp"


SearchForm::SearchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchForm) {
    ui->setupUi(this);
    QStringList filter_expressions = {FILTER_EXP_DATAARRAY, FILTER_EXP_BLOCK,
                                      FILTER_EXP_TAG, FILTER_EXP_MULTITAG,
                                      FILTER_EXP_GROUP, FILTER_EXP_SOURCE};
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
        bool exact = ui->exactCheckBox->isChecked();
        bool case_sensitive = ui->caseSensitivityCheckBox->isChecked();
        std::string term = ui->termEdit->text().toStdString();

        switch (ui->typeComboBox->currentIndex()) {
        case 0: { // looking for data arrays
            std::vector<nix::DataArray> arrays;
            if (ui->fieldComboBox->currentIndex() == 0) {
                for (nix::Block b : this->nix_file.blocks()){
                    std::vector<nix::DataArray> temp = b.dataArrays(NameSearch<nix::DataArray>(term, case_sensitive, exact));
                    arrays.insert(arrays.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 1) {
                for (nix::Block b : this->nix_file.blocks()){
                    std::vector<nix::DataArray> temp = b.dataArrays(TypeSearch<nix::DataArray>(term, case_sensitive, exact));
                    arrays.insert(arrays.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 2) {
                for (nix::Block b : this->nix_file.blocks()){
                    std::vector<nix::DataArray> temp = b.dataArrays(nix::util::IdFilter<nix::DataArray>(term));
                    arrays.insert(arrays.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 3) {
                for (nix::Block b : this->nix_file.blocks()){
                    std::vector<nix::DataArray> temp = b.dataArrays(DefinitionSearch<nix::DataArray>(term, case_sensitive, exact));
                    arrays.insert(arrays.end(), temp.begin(), temp.end());
                }
            }
            for (nix::DataArray da : arrays) {
                this->results.push_back(QVariant::fromValue(da));
            }
            break;
        }
        case 1: {  // looking for blocks
            std::vector<nix::Block> blocks;
            if (ui->fieldComboBox->currentIndex() == 0) {
                blocks = this->nix_file.blocks(NameSearch<nix::Block>(term, case_sensitive, exact));
            } else if (ui->fieldComboBox->currentIndex() == 1) {
                blocks = this->nix_file.blocks(TypeSearch<nix::Block>(term, case_sensitive, exact));
            } else if (ui->fieldComboBox->currentIndex() == 2) {
                blocks = this->nix_file.blocks(nix::util::IdFilter<nix::Block>(term));
            } else if (ui->fieldComboBox->currentIndex() == 3) {
                blocks = this->nix_file.blocks(DefinitionSearch<nix::Block>(term, case_sensitive, exact));
            }
            for (nix::Block b : blocks) {
                this->results.push_back(QVariant::fromValue(b));
            }
            break;
        }
        case 2: { // looking for tags
            std::vector<nix::Tag> tags;
            if (ui->fieldComboBox->currentIndex() == 0) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Tag> temp = b.tags(NameSearch<nix::Tag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 1) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Tag> temp = b.tags(TypeSearch<nix::Tag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 2) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Tag> temp = b.tags(nix::util::IdFilter<nix::Tag>(term));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 3) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Tag> temp = b.tags(DefinitionSearch<nix::Tag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            }
            for (nix::Tag t : tags) {
                this->results.push_back(QVariant::fromValue(t));
            }
            break;
        }
        case 3: { // looking for MultiTags
            std::vector<nix::MultiTag> tags;
            if (ui->fieldComboBox->currentIndex() == 0) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::MultiTag> temp = b.multiTags(NameSearch<nix::MultiTag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 1) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::MultiTag> temp = b.multiTags(TypeSearch<nix::MultiTag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 2) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::MultiTag> temp = b.multiTags(nix::util::IdFilter<nix::MultiTag>(term));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 3) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::MultiTag> temp = b.multiTags(DefinitionSearch<nix::MultiTag>(term, case_sensitive, exact));
                    tags.insert(tags.end(), temp.begin(), temp.end());
                }
            }
            for (nix::MultiTag t : tags) {
                this->results.push_back(QVariant::fromValue(t));
            }
            break;
        }
        case 4: { // looking for Groups
            std::vector<nix::Group> groups;
            if (ui->fieldComboBox->currentIndex() == 0) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Group> temp = b.groups(NameSearch<nix::Group>(term, case_sensitive, exact));
                    groups.insert(groups.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 1) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Group> temp = b.groups(TypeSearch<nix::Group>(term, case_sensitive, exact));
                    groups.insert(groups.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 2) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Group> temp = b.groups(nix::util::IdFilter<nix::Group>(term));
                    groups.insert(groups.end(), temp.begin(), temp.end());
                }
            } else if (ui->fieldComboBox->currentIndex() == 3) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Group> temp = b.groups(DefinitionSearch<nix::Group>(term, case_sensitive, exact));
                    groups.insert(groups.end(), temp.begin(), temp.end());
                }
            }
            for (nix::Group g : groups) {
                this->results.push_back(QVariant::fromValue(g));
            }
            break;
        }
        case 5: { // looking for Sources
            std::vector<nix::Source> srcs;
            if (ui->typeComboBox->currentIndex() == 0) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Source> temp = b.sources(NameSearch<nix::Source>(term, case_sensitive, exact));
                    srcs.insert(srcs.end(), temp.begin(), temp.end());
                }
            } else if (ui->typeComboBox->currentIndex() == 1) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Source> temp = b.sources(TypeSearch<nix::Source>(term, case_sensitive, exact));
                    srcs.insert(srcs.end(), temp.begin(), temp.end());
                }
            } else if (ui->typeComboBox->currentIndex() == 2) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Source> temp = b.sources(nix::util::IdFilter<nix::Source>(term));
                    srcs.insert(srcs.end(), temp.begin(), temp.end());
                }
            } else if (ui->typeComboBox->currentIndex() == 3) {
                for (nix::Block b : this->nix_file.blocks()) {
                    std::vector<nix::Source> temp = b.sources(DefinitionSearch<nix::Source>(term, case_sensitive, exact));
                    srcs.insert(srcs.end(), temp.begin(), temp.end());
                }
            }
            for (nix::Source s : srcs) {
                this->results.push_back(QVariant::fromValue(s));
            }
            break;
        }
        default:
            break;
        }

        emit SearchForm::newResults(this->results);
    }
}


void SearchForm::setNixFile(const nix::File &f) {
    this->nix_file = f;
}


void SearchForm::receiveFocus() {
    ui->termEdit->setFocus();
}


void SearchForm::fieldSelected(int index) {
    if (ui->fieldComboBox->itemText(index) == "id") {
        ui->caseSensitivityCheckBox->setEnabled(false);
        ui->exactCheckBox->setEnabled(false);
    } else {
        ui->caseSensitivityCheckBox->setEnabled(true);
        ui->exactCheckBox->setEnabled(true);
    }
}


void SearchForm::clear() {
    this->results.clear();
    ui->termEdit->clear();
    ui->caseSensitivityCheckBox->setChecked(false);
}

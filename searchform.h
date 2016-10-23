#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QWidget>
#include <nix.hpp>
#include "utils/utils.hpp"


template<typename T>
struct NameSearch : nix::util::Filter<T> {

    const std::string name;
    bool case_sensitive, exact;

    NameSearch(const std::string &str, bool case_sensitive=false, bool exact=false)
            : name(str), case_sensitive(case_sensitive), exact(exact)
        {}

    virtual bool operator()(const T &e) {
        return nixview::util::stringCompare(e.name(), name, case_sensitive, exact);
    }
};


template<typename T>
struct TypeSearch : nix::util::Filter<T> {

    const std::string name;
    bool case_sensitive, exact;

    NameSearch(const std::string &str, bool case_sensitive=false, bool exact=false)
            : name(str), case_sensitive(case_sensitive), exact(exact)
        {}

    virtual bool operator()(const T &e) {
        return nixview::util::stringCompare(e.type(), name, case_sensitive, exact);
    }
};


template<typename T>
struct DefinitionSearch : nix::util::Filter<T> {

    const std::string name;
    bool case_sensitive, exact;

    NameSearch(const std::string &str, bool case_sensitive=false, bool exact=false)
            : name(str), case_sensitive(case_sensitive), exact(exact)
        {}

    virtual bool operator()(const T &e) {
        return nixview::util::stringCompare(e.definition(), name, case_sensitive, exact);
    }
};


namespace Ui {
class SearchForm;
}

class SearchForm : public QWidget
{
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent = 0);
    ~SearchForm();

    void clear();
    void setNixFile(const nix::File &f);

signals:
    void newResults(std::vector<QVariant>);

public slots:
    void go();

private:
    Ui::SearchForm *ui;
    std::vector<QVariant> results;
    nix::File nix_file;
};

#endif // SEARCHFORM_H
// TODO invalidate nix file, if file was closed

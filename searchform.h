#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QWidget>
#include <nix.hpp>

namespace Ui {
class SearchForm;
}

class SearchForm : public QWidget
{
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent = 0);
    ~SearchForm();

    void setNixFile(const nix::File &f);

private:
    Ui::SearchForm *ui;
    nix::File nix_file;
};

#endif // SEARCHFORM_H

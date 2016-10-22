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

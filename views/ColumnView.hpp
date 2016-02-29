#ifndef COLUMNVIEW_HPP
#define COLUMNVIEW_HPP

#include <QWidget>
#include <QColumnView>
#include "model/NixDataModel.hpp"

namespace Ui {
class ColumnView;
}

class ColumnView : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnView(NixDataModel*, QWidget *parent = 0);
    ~ColumnView();

private:
    NixDataModel* nix_model;
    Ui::ColumnView *ui;
};

#endif // COLUMNVIEW_HPP

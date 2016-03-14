#ifndef COLUMNVIEW_HPP
#define COLUMNVIEW_HPP

#include <QWidget>
#include <QColumnView>
#include "model/NixDataModel.hpp"
#include "filter/NixProxyModel.hpp"

namespace Ui {
class ColumnView;
}

class ColumnView : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnView(QWidget *parent=0);
    explicit ColumnView(NixProxyModel*, QWidget *parent = 0);
    ~ColumnView();

    QColumnView* get_column_view();
    void set_proxy_model(NixProxyModel *proxy_model);

private:
    NixProxyModel* nix_proxy_model;
    QColumnView* qcv;
    Ui::ColumnView *ui;
};

#endif // COLUMNVIEW_HPP

#ifndef COLUMNVIEW_HPP
#define COLUMNVIEW_HPP

#include <QWidget>
#include <QColumnView>
#include <QSortFilterProxyModel>

namespace Ui {
class ColumnView;
}

class ColumnView : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnView(QWidget *parent=0);
    explicit ColumnView(QSortFilterProxyModel*, QWidget *parent = 0);
    ~ColumnView();

    QColumnView* get_column_view();
    void set_proxy_model(QSortFilterProxyModel *proxy_model);

private:
    QSortFilterProxyModel* nix_proxy_model;
    QColumnView* qcv;
    Ui::ColumnView *ui;
};

#endif // COLUMNVIEW_HPP
